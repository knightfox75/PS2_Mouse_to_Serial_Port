/*******************************************************************************
  
    Adaptador de mouse PS2 a puerto serie RS-232

    Datos de configuracion

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



#ifndef __CONFIG_H
#define __CONFIG_H


/*** Includes ***/
#include <Arduino.h>



/*** Define los PINS a usar en la placa Arduino ***/
const uint8_t PIN_MOUSE_CLOCK = 2;          // Señal de reloj del protocolo PS/2 (Pin 5 del conector PS/2)
const uint8_t PIN_MOUSE_DATA = 5;           // Datos del protocolo PS/2 (Pin 1 del conector PS/2)

const uint8_t PIN_RTS_SIGNAL = 7;           // Señal RTS del puerto serie (Pin 7 del conector DB9) *1

const uint8_t PIN_MSG_LED = 8;              // LED indicador de estado

// *1 - No conectar NUNCA este pin del puerto serie directamente a un pin de la placa Arduino



/*** Define los datos de configuracion del protocolo PS/2 ***/
const uint8_t READ_LOOPS = 4;               // Numero de lecturas del puerto PS/2 por ciclo [4]
const uint8_t SAMPLE_RATE = 100;            // Samples/seg [10, 20, 40, 60, 80, >> 100 <<, 200]
const uint8_t RESOLUTION = 0x02;            // Resolucion [0x00 = 1 count/mm, 0x01 = 2 count/mm, >> 0x02 = 4 count/mm <<, 0x03 = 8 count/mm]


/*** Define los datos de configuracion del protocolo SERIE ***/
const uint32_t SERIAL_BAUD_RATE = 1200;
const uint8_t SERIAL_OPTIONS = SERIAL_7N1;


/*** Configuracion adicional del mouse ***/
const int8_t MOUSE_BLINK_DELAY = 3;         // Espera entre parpadeos [3]
const float MOUSE_DEFAULT_SPEED = 0.5f;     // Factor de escala en la velocidad [0.5f]
const float MOUSE_SPEED_CHANGE = 0.05f;     // Factor de cambio [0.05f]
const float MOUSE_MIN_SPEED = 0.1f;         // Velocidad minima [0.1f]
const float MOUSE_MAX_SPEED = 1.0f;         // Valocidad maxima [1.0f]
const float SAVE_HELD_TIME = 5000;          // Tiempo que se requiere para el guardado de datos en la eeprom [5s]
const float RESTORE_HELD_TIME = 10000;       // Tiempo que se requiere para la restauracion y guardado de datos en la eeprom [10s]
const uint8_t CHK0 = 0x01, CHK2 = 0x04;     // Checksum de datos de la eeprom 


#endif
