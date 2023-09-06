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



#ifndef __PS2_MOUSE_INTERFACE_H
#define __PS2_MOUSE_INTERFACE_H


/*** Includes ***/
// Arduino
#include <Arduino.h>
// Proyecto
#include "config.h"



/*** Declaracion de la clase ***/
class Ps2MouseInterface {


    public:

        // Contructor & destructor
        Ps2MouseInterface(uint8_t _clock_pin, uint8_t _data_pin, uint8_t _sample_rate, uint8_t _resolution);
        ~Ps2MouseInterface();

        // Inicia el puerto/raton
        void PowerUp();         // Indica al raton que debe encenderse (requerido en algunos modelos)
        bool Init();            // Inicia la comunicacion con el puerto PS/2

        // Obten los datos registrados por el raton
        void GetReport(int16_t data[]);     // Lee los 3 bytes de datos y devuelvelos en el array DATA
        void EmptyBuffer();                 // Lee los 3 bytes de datos, sin devolverlos


    private:

        // Propiedades internas de configuracion
        uint8_t clock_pin;          // Pin asignado a la señal de reloj
        uint8_t data_pin;           // Pin asignado a los datos
        uint8_t sample_rate;        // Sample rate [10, 20, 40, 60, 80, 100, 200]
        uint8_t resolution;         // Resolucion [0x00 = 1 count/mm, 0x01 = 2 count/mm, 0x02 = 4 count/mm, 0x03 = 8 count/mm]
        // Parametros de funcionamiento
        const int32_t RTS_CLOCK_TIME = 100;     // Tiempo con la señal de clock a 0 para solicitar la transmision (al menos 100uS)
        const int32_t RTR_CLOCK_TIME = 50;      // Tiempo con la señal de clock a 1 para solicitar la recepcion (al menos 50uS)
        const int32_t RTS_PAUSE_TIME = 100;     // Tiempo con la señal de clock a 0 para pausar la transmision (al menos 100uS)
        const int32_t CLOCK_FALLOUT = 5;        // Tiempo de caida de la señal de reloj (5uS)
        const int32_t POWER_UP_DELAY = 500;     // Espera para que el mouse realice el power up

        // Definicion de comandos
        const uint8_t CMD_RESET = 0xFF;
        const uint8_t CMD_RESEND = 0xFE;
        const uint8_t CMD_ERROR = 0xFC;
        const uint8_t CMD_ACKNOWLEDGE = 0xFA;
        const uint8_t CMD_SET_DEFAULTS = 0xF6;
        const uint8_t CMD_DISABLE_DATA_REPORTING = 0xF5;
        const uint8_t CMD_ENABLE_DATA_REPORTING = 0xF4;
        const uint8_t CMD_SET_SAMPLE_RATE = 0xF3;
        const uint8_t CMD_GET_DEVICE_ID = 0xF2;
        const uint8_t CMD_SET_REMOTE_MODE = 0xF0;
        const uint8_t CMD_SET_WRAP_MODE = 0xEE;
        const uint8_t CMD_RESET_WRAP_MODE = 0xEC;
        const uint8_t CMD_READ_DATA = 0xEB;
        const uint8_t CMD_SET_STREAM_MODE = 0xEA;
        const uint8_t CMD_STATUS_REQUEST = 0xE9;
        const uint8_t CMD_SET_RESOLUTION = 0xE8;
        const uint8_t CMD_SET_SCALING_2_1 = 0xE7;
        const uint8_t CMD_SET_SCALING_1_1 = 0xE6;
        const uint8_t CMD_SELF_TEST = 0xAA;

        // Tipos de dispositivos
        const uint8_t STD_MOUSE = 0x00;
        const uint8_t INTELLI_MOUSE = 0x03;

        // Variables para el uso interno
        uint16_t bits_buffer;       // Paquete de 11 bits para el envio de datos
        uint8_t mouse_id;           // ID del mouse
        uint8_t data_length;        // Tamaño del paquete de datos

        // PULL-UP y PULL-DOWN de los pins
        void PinPullUp(uint8_t pin);
        void PinPullDown(uint8_t pin);

        // Escribe un Byte en el pin de datos
        void WriteByte(uint8_t dt);
        // Escribe un bit en el pin de datos
        void WriteBit(bool bit);

        // Lee un byte del pin de datos
        uint8_t ReadByte();
        // Lee un bit del pin de datos
        bool ReadBit();

        // Habilita/deshabilita el envio de datos en modo stream
        bool EnableDataReporting();
        bool DisableDataReporting();

        // Sequencia de incio de IntelliMouse
        bool IntelliMouseCheck();

};



#endif
