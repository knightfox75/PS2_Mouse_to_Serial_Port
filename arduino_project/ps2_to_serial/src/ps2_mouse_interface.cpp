/*******************************************************************************
  
    Interfaz de raton PS/2 para Arduino
    Version 1.0.0-STABLE

    Proyecto iniciado el Martes, 02 de Septiembre del 2023
    por Cesar Rincon "NightFox"

    https://nightfoxandco.com
    contact@nightfoxandco.com

    Basado en los siguientes proyectos de GitHub
    https://github.com/kristopher/PS2-Mouse-Arduino

    Bibliografia
    https://isdaman.com/alsos/hardware/mouse/ps2interface.htm
    https://www.burtonsys.com/ps2_chapweske.htm
    https://wiki.osdev.org/PS/2_Mouse
  

    "Interfaz de raton PS/2 para Arduino" is under MIT License

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
// Proyecto
#include "config.h"
#include "ps2_mouse_interface.h"



/*** Constructor ***/
Ps2MouseInterface::Ps2MouseInterface(uint8_t _clock_pin, uint8_t _data_pin, uint8_t _sample_rate, uint8_t _resolution) {

    clock_pin = _clock_pin;
    data_pin = _data_pin;
    sample_rate = _sample_rate;
    resolution = _resolution;
    data_length = 3;

}



/*** Destructor ***/
Ps2MouseInterface::~Ps2MouseInterface() {

}



/*** Indica al raton que debe encenderse (requerido en algunos modelos) ***/
void Ps2MouseInterface::PowerUp() {

    // Secuencia para el power-up del raton
    delay(POWER_UP_DELAY);
    PinPullUp(clock_pin);
    PinPullUp(data_pin);
    delay(POWER_UP_DELAY);

}



/*** Inicia el puerto/raton ***/
bool Ps2MouseInterface::Init() {

    // Rutina de inicializacion estandar
    for (uint8_t i = 0; i < 3; i++) {
        // Envia el comando reset
        WriteByte(CMD_RESET);
        // Lee el "reconocido"
        if (ReadByte() != CMD_ACKNOWLEDGE) return false;
        // Lee el "ok"
        if (ReadByte() != CMD_SELF_TEST) return false;
        // Lee el "mouse ID"
        mouse_id = ReadByte();
    }

    // Fuerza los valores por defecto del protocolo ps/2
    // Sampling rate = 100, Resolution = 4 counts/mm, Scaling = 1:1, Disable Data Reporting,
    // resetea los contadores y entra en modo stream
    WriteByte(CMD_SET_DEFAULTS);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // Verifica si el raton conectado tiene capacidades extendidas
    if (!IntelliMouseCheck()) return false;
    data_length = (mouse_id == INTELLI_MOUSE) ? 4:3;

    // Sample Rate
    WriteByte(CMD_SET_SAMPLE_RATE);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;
    WriteByte(sample_rate);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // Scaling
    WriteByte(CMD_SET_SCALING_1_1);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // Resolution
    WriteByte(CMD_SET_RESOLUTION);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;
    WriteByte(resolution);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // Indica al raton que empiece con el envio de datos
    WriteByte(CMD_ENABLE_DATA_REPORTING);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // Informa del proceso de inicio correcto
    return true;

}



/*** Pon un pin en PULL-UP [1, alta impedancia] ***/
void Ps2MouseInterface::PinPullUp(uint8_t pin) {

    pinMode(pin, INPUT_PULLUP);

}



/*** Pon un pin en PULL-DOWN [0] ***/
void Ps2MouseInterface::PinPullDown(uint8_t pin) {

    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);

}



/*** Escribe un Byte en el pin de datos ***/
void Ps2MouseInterface::WriteByte(uint8_t dt) {
    
    // Calcula la paridad
    uint8_t parity = 0x01;
    uint8_t d = dt;
    while(d != 0) {
        parity ^= (d & 0x01);
        d >>= 1;
    }

    // Prepara el paquete de datos (11 bits)
    bits_buffer = 0x00;                     // BIT inicial [0], siempre 0
    bits_buffer |= ((dt & 0xFF) << 1);      // BITS de datos [1 al 8]
    bits_buffer |= ((parity & 0x01) << 9);  // BIT de paridad [9]
    bits_buffer |= (1 << 10);               // BIT de parada [10], siempre 1

    // Indica al puerto que quieres enviar datos
    PinPullDown(clock_pin);
    delayMicroseconds(RTS_CLOCK_TIME);

    // Envia el BIT de inicio [0]
    PinPullDown(data_pin);
    PinPullUp(clock_pin);
    while(digitalRead(clock_pin));

    // Envio del paquete de datos y paridad (bits del 1 al 9)
    for (uint8_t i = 1; i <= 9; i ++) WriteBit((bits_buffer >> i) & 0x01);

    // Libera la linea de datos (bit de parada [10])
    PinPullUp(data_pin);
    while(digitalRead(data_pin));
    while(digitalRead(clock_pin));

    // Espera a que el dispositivo vuelva al modo IDLE
    while (!digitalRead(clock_pin) || !digitalRead(data_pin));

}



/*** Escribe un bit en el pin de datos ***/
void Ps2MouseInterface::WriteBit(bool bit) {

    if (bit) {
        PinPullUp(data_pin);
    } else {
        PinPullDown(data_pin);
    }

    // Espera un ciclo de reloj
    while (!digitalRead(clock_pin));
    while (digitalRead(clock_pin));

}



/*** Lee un byte del pin de datos ***/
uint8_t Ps2MouseInterface::ReadByte() {

    // Datos a devolver
    uint8_t dt = 0xFF;

    // Indica al puerto que quieres recibir datos
    PinPullUp(clock_pin);
    PinPullUp(data_pin);
    delayMicroseconds(RTR_CLOCK_TIME);

    // Lee el paquete de datos
    bits_buffer = 0x00;
    for (uint8_t i = 0; i < 11; i ++) bits_buffer |= ReadBit() ? (1 << i):0x00;
    
    // Indica al puerto que espera para el envio del siguiente paquete de datos
    PinPullDown(clock_pin);
    delayMicroseconds(RTS_PAUSE_TIME);

    // Devuelve los datos (devulve 0 si os bits de inicio y parada no son correctos)
    if (!(bits_buffer & 0x01) && ((bits_buffer >> 10) & 0x01)) dt = ((bits_buffer >> 1) & 0xFF);
    return dt;

}



/*** Lee un bit del pin de datos ***/
bool Ps2MouseInterface::ReadBit() {

    while(digitalRead(clock_pin));
    bool b = digitalRead(data_pin);
    while(!digitalRead(clock_pin));
    return b;

}



/*** Habilita el envio de datos en modo stream ***/
bool Ps2MouseInterface::EnableDataReporting() {

    WriteByte(CMD_ENABLE_DATA_REPORTING);
    return (ReadByte() == CMD_ACKNOWLEDGE) ? true:false;

}




/*** Deshabilita el envio de datos en modo stream ***/
bool Ps2MouseInterface::DisableDataReporting() {

    WriteByte(CMD_DISABLE_DATA_REPORTING);
    return (ReadByte() == CMD_ACKNOWLEDGE) ? true:false;

}



/*** Obten los datos registrados por el raton ***/
void Ps2MouseInterface::GetReport(int16_t data[]) {

    // Comando solicitando el report, si falla, no alteres la informacion del array de datos
    WriteByte(CMD_READ_DATA);
    if (ReadByte() != CMD_ACKNOWLEDGE) return;

    // Almacena los bytes con la informacion del reporte
    uint8_t byte[4];
    for (uint8_t i = 0; i < data_length; i ++) byte[i] = ReadByte();

    // Copia los datos leeidos con el formato adecuado al array
    for (uint8_t i = 0; i < data_length; i ++) data[i] = (byte[i] & 0x00FF);
    if (data[0] & 0x10) data[1] |= 0xFF00;
    if (data[0] & 0x20) data[2] |= 0xFF00;

    // IntelliMouse
    if (data_length == 4) {
        if (byte[3] > 0) {
            if (byte[3] & 0xF0) {
                data[3] = -1;
            } else {
                data[3] = 1;
            }
        }
    } else {
        data[3] = 0;
    }

}



/*** Lee los 3 bytes de datos, sin devolverlos (vacia el buffer) ***/
void Ps2MouseInterface::EmptyBuffer() {

    // Comando solicitando el report, si falla, no alteres la informacion del array de datos
    WriteByte(CMD_READ_DATA);
    if (ReadByte() != CMD_ACKNOWLEDGE) return;

    // Lee los 3 bytes de datos
    for (uint8_t i = 0; i < data_length; i ++) ReadByte();

}



/*** Sequencia de incio de IntelliMouse ***/
bool Ps2MouseInterface::IntelliMouseCheck() {

    // 200
    WriteByte(CMD_SET_SAMPLE_RATE);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;
    WriteByte(0xC8);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // 100
    WriteByte(CMD_SET_SAMPLE_RATE);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;
    WriteByte(0x64);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // 80
    WriteByte(CMD_SET_SAMPLE_RATE);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;
    WriteByte(0x50);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;

    // Device ID
    WriteByte(CMD_GET_DEVICE_ID);
    if (ReadByte() != CMD_ACKNOWLEDGE) return false;
    mouse_id = ReadByte();

    // Test completado
    return true;

}