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
// Librerias
#include "lib/PS2Mouse/PS2Mouse.h"
// Proyecto
#include "config.h"
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
    // Inicia el protocolo PS/2
    SetPs2();
    // Inicia el protocolo SERIE
    SetSerial();

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
    // estos datos se leeran 4 veces para compensar la diferencia de velocidades
    for (uint8_t i = 0; i < 4; i ++) ReadMouse();

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

    // Registra el estado de los botones para la siguiente iteracion
    mouse.last.left = mouse.button.left;
    mouse.last.right = mouse.button.right;
    mouse.last.middle = mouse.button.middle;

}



/*** Metodo para establecer los valores por defecto ***/
void MainCode::SetDefaults() {

    // Mouse
    mouse.position.x = 0;
    mouse.position.y = 0;
    mouse.delta.x = 0;
    mouse.delta.y = 0;
    mouse.button.left = false;
    mouse.button.right = false;
    mouse.button.middle = false;
    mouse.last.left = false;
    mouse.last.right = false;
    mouse.last.middle = false;
    for (uint8_t i = 0; i < 4; i ++) mouse.packet[i] = 0;
    mouse.packet_size = 3;
    mouse.event = false;
    mouse.led = 0;

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

    ps2mouse.initialize();  
    ps2mouse.set_sample_rate(PS2_SAMPLE_RATE);
    ps2mouse.set_scaling_1_1();

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
    ps2mouse.report(mouse.data);

    // Actualiza el desplazamiento del cursor
    mouse.position.x += mouse.data[1];
    mouse.position.y -= mouse.data[2];

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
        if (mouse.button.middle && !mouse.last.middle) {
            ps2mouse.report(mouse.data);
            ps2mouse.report(mouse.data);
        }
    }

}



/*** Matodo para el analisis y preparacion de los datos para su envio ***/
void MainCode::ParseData() {

    // Divide entre dos los valores de desplazamiento para suavizarlo
    mouse.delta.x = (mouse.position.x / 2);
    mouse.delta.y = (mouse.position.y / 2);

    // Si se detecta desplazamiento, ajusta los valores delta, reinicia los contadores y registra el evento
    if (mouse.delta.x != 0) {
        mouse.position.x = 0;
        mouse.event |= true;
        if (mouse.delta.x < -127) mouse.delta.x = -127;
        if (mouse.delta.x > 127) mouse.delta.x = 127;
    }
    if (mouse.delta.y != 0) {
        mouse.position.y = 0;
        mouse.event |= true;
        if (mouse.delta.y < -127) mouse.delta.y = -127;
        if (mouse.delta.y > 127) mouse.delta.y = 127;
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
        if (mouse.button.left || mouse.button.right || mouse.button.middle) {
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
