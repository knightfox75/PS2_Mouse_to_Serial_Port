/*******************************************************************************
  
    Adaptador de mouse PS2 a puerto serie RS-232

    Clase principal del proyecto

    Proyecto iniciado el Martes, 25 de Julio del 2023
    por Cesar Rincon "NightFox"

    https://nightfoxandco.com
    contact@nightfoxandco.com

    Basado en los siguientes proyectos de GitHub
    https://github.com/Lameguy64/ps2serial
    https://github.com/kristopher/PS2-Mouse-Arduino
  

    "Adaptador de mouse PS2 a puerto serie RS-232" is under MIT License

    Copyright (c) 2023 by Cesar Rincon "NightFox"

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal	in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all	copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER	LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
*******************************************************************************/



/*** Includes ***/
// Arduino
#include <Arduino.h>
#include <EEPROM.h>
// Proyecto
#include "config.h"
#include "ps2_mouse_interface.h"
#include "maincode.h"



/*** Constructor ***/
MainCode::MainCode() {

}



/*** Destructor ***/
MainCode::~MainCode() {

}



/*** Metodo para la configuracion inicial ***/
void MainCode::Start() {

    // Establece los valores por defecto
    SetDefaults();
    // Establece los pins
    SetPins();
    // Indica al raton PS/2 que debe encenderse
    ps2mouse.PowerUp();
    // Inicia el protocolo PS/2
    SetPs2();
    // Inicia el protocolo SERIE
    SetSerial();
    // Carga la configuracion por defecto del raton (velocidad)
    LoadMouseSpeed();

    // 3 parpadeos para indicar el bootup correcto
    for (uint8_t i = 0; i < 3; i ++) {
        digitalWrite(PIN_MSG_LED, HIGH);
        delay(100);
        digitalWrite(PIN_MSG_LED, LOW);
        delay(100);
    }
 
}



/*** Metodo del bucle de ejecucion del programa ***/
void MainCode::Update() {

    // Prepara los datos para esta iteracion
    mouse.button.left = false;
    mouse.button.right = false;
    mouse.button.middle = false;
    mouse.event = false;

    // Lectura de los datos del puerto PS/2
    // Dado que el puerto PS/2 es mucho mas rapido que el puerto SERIE,
    // estos datos se leeran "READ_LOOPS" veces para compensar la diferencia de velocidades
    for (uint8_t i = 0; i < READ_LOOPS; i ++) ReadMouse();

    // Analisis y preparacion de los datos recogidos
    ParseData();
    // LED de actividad
    ActivityLed();
    // Codifica los datos recogidos
    EncodeData();
    // Envia, si es necesario, el paquete de datos por el puero serie
    SendData();
    // Monitoreo de la señal RTS
    RtsStatusMonitor();

    // Guardado de datos?
    SaveMouseSpeed();

    // Registra el estado de los botones para la siguiente iteracion
    mouse.last.left = mouse.button.left;
    mouse.last.right = mouse.button.right;
    mouse.last.middle = mouse.button.middle;
    mouse.last.wheel = mouse.delta.wheel;

}



/*** Metodo para establecer los valores por defecto ***/
void MainCode::SetDefaults() {

    // Mouse
    mouse.position.x = 0;
    mouse.position.y = 0;
    mouse.position.wheel = 0;
    mouse.delta.x = 0;
    mouse.delta.y = 0;
    mouse.delta.wheel = 0;
    mouse.button.left = false;
    mouse.button.right = false;
    mouse.button.middle = false;
    mouse.last.left = false;
    mouse.last.right = false;
    mouse.last.middle = false;
    mouse.last.wheel = 0;
    for (uint8_t i = 0; i < 4; i ++) mouse.packet[i] = 0;
    mouse.packet_size = 3;
    mouse.event = false;
    mouse.led = 0;
    mouse.speed = MOUSE_DEFAULT_SPEED;
    mouse.save_timer = (millis() + SAVE_HELD_TIME);
    mouse.save_flag = false;
    mouse.restore_timer = (millis() + RESTORE_HELD_TIME);
    mouse.restore_flag = false;

    // RS-232
    rs232.rts = false;

}



/*** Metodo para configurar los PINS de la placa Arduino ***/
void MainCode::SetPins() {

    // Señal RTS del puerto serie
    pinMode(PIN_RTS_SIGNAL, INPUT);

    // LED de estado
    pinMode(PIN_MSG_LED, OUTPUT);
    digitalWrite(PIN_MSG_LED, LOW);

}



/*** Metodo para configurar el protocolo PS/2 ***/
void MainCode::SetPs2() {

    digitalWrite(PIN_MSG_LED, HIGH);
    
    bool ps2_ready = false;
    do {
        // Intenta iniciar el mouse ps/2, si no se consigue, reintentalo en 1 segundo
        ps2_ready |= ps2mouse.Init();
        if (!ps2_ready) {
            delay(500);
            digitalWrite(PIN_MSG_LED, LOW);
            delay(500);
            digitalWrite(PIN_MSG_LED, HIGH);
        }
    } while (!ps2_ready);

    digitalWrite(PIN_MSG_LED, LOW);

}



/*** Metodo para configurar el protocolo SERIE de la placa Arduino ***/
void MainCode::SetSerial() {

    digitalWrite(PIN_MSG_LED, HIGH);

    Serial.begin(SERIAL_BAUD_RATE, SERIAL_OPTIONS);
    while(!Serial);

    digitalWrite(PIN_MSG_LED, LOW);

}



/*** Metodo para la lectura de los datos del raton PS/2 ***/
void MainCode::ReadMouse() {

    // Lectura de los datos actuales
    ps2mouse.GetReport(mouse.data);

    // Actualiza el desplazamiento del cursor
    mouse.position.x += mouse.data[1];
    mouse.position.y -= mouse.data[2];

    // Actualiza la rueda (si esta presente)
    mouse.position.wheel += mouse.data[3];

    // Boton Izquierdo
    if (mouse.data[0] & 0x01) {
        mouse.button.left |= true;
        mouse.event |= true;
    }

    // Boton Derecho
    if (mouse.data[0] & 0x02) {
        mouse.button.right |= true;
        mouse.event |= true;
    }

    // Boton Central
    if (mouse.data[0] & 0x04) {
        mouse.button.middle |= true;
        mouse.event |= true;
        // Compensacion de retardo por la lectura de paquetes extra al pulsar el boton central
        if (mouse.button.middle && !mouse.last.middle) ps2mouse.EmptyBuffer();
    }

}



/*** Matodo para el analisis y preparacion de los datos para su envio ***/
void MainCode::ParseData() {

    // Rueda del raton
    mouse.delta.wheel = mouse.position.wheel;

    // Ajusta la velocidad del cursor con la rueda (si esta disponible)
    if (mouse.delta.wheel != mouse.last.wheel) {
        if (mouse.delta.wheel > 0) {
            mouse.speed += MOUSE_SPEED_CHANGE;
            if (mouse.speed > MOUSE_MAX_SPEED) mouse.speed = MOUSE_MAX_SPEED;
        }
        if (mouse.delta.wheel < 0) {
            mouse.speed -= MOUSE_SPEED_CHANGE;
            if (mouse.speed < MOUSE_MIN_SPEED) mouse.speed = MOUSE_MIN_SPEED;
        }
    }

    // Ajuste de velocidad
    mouse.delta.x = (mouse.position.x * mouse.speed);
    mouse.delta.y = (mouse.position.y * mouse.speed);

    // Si se detecta desplazamiento, ajusta los valores delta, reinicia los contadores y registra el evento
    if (mouse.delta.x != 0) {
        mouse.position.x = 0;
        mouse.event |= true;
        if (mouse.delta.x < -255) mouse.delta.x = -255;
        if (mouse.delta.x > 255) mouse.delta.x = 255;
    }
    if (mouse.delta.y != 0) {
        mouse.position.y = 0;
        mouse.event |= true;
        if (mouse.delta.y < -255) mouse.delta.y = -255;
        if (mouse.delta.y > 255) mouse.delta.y = 255;
    }

    // Captura el movimiento de la rueda
    if (mouse.delta.wheel != 0) {
        mouse.position.wheel = 0;
        mouse.event |= true;
        if (mouse.delta.wheel < -7) mouse.delta.wheel = -7;
        if (mouse.delta.wheel > 7) mouse.delta.wheel = 7;
    }

    // Captura los eventos por el cambio de estado al pulsar/soltar cualquier boton del mouse
    mouse.event |= (mouse.button.left != mouse.last.left);
    mouse.event |= (mouse.button.right != mouse.last.right);
    mouse.event |= (mouse.button.middle != mouse.last.middle);

}



/*** Metodo el LED de actividad del raton ***/
void MainCode::ActivityLed() {

    // Led de actividad
    if (mouse.event) {
        if (mouse.restore_flag) {
            digitalWrite(PIN_MSG_LED, HIGH);
            mouse.led = MOUSE_BLINK_DELAY;   
        } else if (mouse.save_flag) {
            digitalWrite(PIN_MSG_LED, LOW);
            mouse.led = MOUSE_BLINK_DELAY;
        } else if (mouse.button.left || mouse.button.right || mouse.button.middle || (mouse.delta.wheel != 0)) {
            digitalWrite(PIN_MSG_LED, HIGH);
            mouse.led = MOUSE_BLINK_DELAY;
        } else {
            mouse.led --;
            if (mouse.led <= 0) {
                mouse.led = MOUSE_BLINK_DELAY;
                digitalWrite(PIN_MSG_LED, !digitalRead(PIN_MSG_LED));
            }
        }
    } else {
        mouse.led = MOUSE_BLINK_DELAY;
        digitalWrite(PIN_MSG_LED, LOW);
    }

}



/*** Metodo para codificacion del paquete de datos ***/
void MainCode::EncodeData() {

    // Si no hay eventos registrados sal
    if (!mouse.event) return;

    // Prepara los datos de codificacion
    uint8_t x = mouse.delta.x;
    uint8_t y = mouse.delta.y;
    uint8_t rb = mouse.button.right ? 1 : 0;
    uint8_t lb = mouse.button.left ? 1 : 0;
    uint8_t mb = mouse.button.middle ? 1 : 0;
    mouse.packet_size = 3;

    // Codificacion del byte 0 del paquete
    mouse.packet[0] =
        ((x >> 6) & 0x03)                   // Ultimos 2 bits del delta X
        |
        (((y >> 6) & 0x03) << 2)            // Ultimos 2 bits del delta Y
        |
        ((rb << 4) | (lb << 5) | 0x40)      // Estados de los botones derecho e izquierdo y el ID de paquete
    ;

    // Codificacion del byte 1 del paquete
    mouse.packet[1] = (x & 0x3F);           // Primeros 6 bits del delta X
    // Codificacion del byte 2 del paquete
    mouse.packet[2] = (y & 0x3F);           // Primeros 6 bits del delta Y

    // Codificacion del byte 3 del paquete (estado del boton central)
    if (mouse.button.middle) {      // Envia el 4º paquete mientras el boton central este presionado
        mouse.packet[3] = 0x20;
        mouse.packet_size = 4;
    } else {                        // Envia el 4º paquete una vez cuando se suelte el boton central
        if (mouse.last.middle && !mouse.button.middle) {
            mouse.packet[3] = 0x00;
            mouse.packet_size = 4;
        }
    }

}



/*** Metodo para el envio del paquete de datos al puerto serie ***/
void MainCode::SendData() {

    // Fase 1 - Envio del paquete de datos en caso de evento del raton
    if (mouse.event) Serial.write(mouse.packet, mouse.packet_size);

    // Fase 2 - Envio de un paquete adicional en caso de evento del boton central
    if (mouse.button.middle != mouse.last.middle) {
        mouse.packet[0] = 0x40;
        mouse.packet[1] = 0x00;
        mouse.packet[2] = 0x00;
        mouse.packet_size = 3;
        Serial.write(mouse.packet, mouse.packet_size);
    }

}



/*** Metodo para el monitoreo del estado de la señal RTS del puerto serie ***/
void MainCode::RtsStatusMonitor() {

    if (digitalRead(PIN_RTS_SIGNAL)) {

        // Si no se ha recibido la señal previamente
        if (!rs232.rts) {

            // Envia los bytes de inicializacion del protocolo
            delay(14);
            Serial.write(0x4D);         // char: 'M'
            delay(63);
            Serial.write(0x33);         // char: '3'

            // Reinicia el estado de los botones
            mouse.button.left = false;
            mouse.button.right = false;
            mouse.button.middle = false;

            // Reinicia el registro de posicion
            mouse.position.x = 0;
            mouse.position.y = 0;
            mouse.position.wheel = 0;

            // Registra el cambio de flag del RTS
            rs232.rts = true;

            // Indicalo en el led de activadad
            digitalWrite(PIN_MSG_LED, HIGH);
            delay(500);
            digitalWrite(PIN_MSG_LED, LOW);

        }

    } else {

        rs232.rts = false;      // Flag de señal recibida abajo

    }

}


/*** Guarda la configuracion de velocidad ***/
void MainCode::SaveMouseSpeed() {

    // Si no estan presionados ambos botones, reinicia los temporizadores y sal
    if (!mouse.button.left || !mouse.button.right) {
        mouse.save_timer = (millis() + SAVE_HELD_TIME);
        mouse.restore_timer = (millis() + RESTORE_HELD_TIME);
        // Reinicia flags de guardado si se sueltan ambos botones
        if (!mouse.button.left && !mouse.button.right) {
            mouse.save_flag = false;
            mouse.restore_flag = false;
        }
        // Sal
        return;
    }

    // Datos
    bool save_now = false;
    uint8_t data = 0;

    // Guardado de los parametros actuales
    if (!mouse.save_flag) {
        if ((millis() >= mouse.save_timer)) { 
            data = (uint8_t)(mouse.speed * 100.0f);
            mouse.save_flag = true;
            save_now = true;
        }
    }

    // Reinicio y guardado de los parametros
    if (!mouse.restore_flag) {
        if ((millis() >= mouse.restore_timer)) { 
            mouse.speed = MOUSE_DEFAULT_SPEED;
            data = (uint8_t)(mouse.speed * 100.0f);
            mouse.restore_flag = true;
        }
    }

    // Guarda los parametros en la EEPROM si se solicita
    if (save_now) {
        EEPROM.write(0x00, CHK0);
        EEPROM.write(0x01, data);
        EEPROM.write(0x02, CHK2);
    }

}



/*** Lee la configuracion de velocidad ***/
void MainCode::LoadMouseSpeed() {

    uint8_t b0 = EEPROM.read(0x00);
    uint8_t data = EEPROM.read(0x01);
    uint8_t b2 = EEPROM.read(0x02);

    if ((b0 != CHK0) || (b2 != CHK2)) {
        // Si los datos son invalidos, crealos por defecto
        data = (uint8_t)(MOUSE_DEFAULT_SPEED * 100.0f);
        EEPROM.write(0x00, CHK0);
        EEPROM.write(0x01, data);
        EEPROM.write(0x02, CHK2);
    }

    mouse.speed = ((float)data / 100.0f);
    if (mouse.speed < MOUSE_MIN_SPEED) mouse.speed = MOUSE_MIN_SPEED;
    if (mouse.speed > MOUSE_MAX_SPEED) mouse.speed = MOUSE_MAX_SPEED;

}
