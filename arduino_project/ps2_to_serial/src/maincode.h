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



#ifndef __MAINCODE_H
#define __MAINCODE_H


/*** Includes ***/
// Arduino
#include <Arduino.h>
// Proyecto
#include "config.h"
#include "ps2_mouse_interface.h"



/*** Declaracion de la clase ***/
class MainCode {


    public:

        // Contructor & destructor
        MainCode();
        ~MainCode();

        // Metodo para la configuracion inicial
        void Start();

        // Metodo del bucle de ejecucion del programa
        void Update();


    private:

        // Objeto de la clase PS2Mouse (Acceso al raton PS/2)
        Ps2MouseInterface ps2mouse = Ps2MouseInterface(PIN_MOUSE_CLOCK, PIN_MOUSE_DATA, SAMPLE_RATE, RESOLUTION);

        // Variables de control de los datos del raton PS/2
        struct {
            struct {
                bool left;
                bool right;
                bool middle;
            } button;
            struct {
                bool left;
                bool right;
                bool middle;
                int32_t wheel;
            } last;
            struct {
                int32_t x;
                int32_t y;
                int32_t wheel;
            } position;
            struct {
                int32_t x;
                int32_t y;
                int32_t wheel;
            } delta;
            uint8_t packet[4];
            uint8_t packet_size;
            int16_t data[4];            // [0] - Status bits, [1] - Movement X, [2] - Movement Y, [4] - Movement Z
            bool event;                 // Evento de raton
            int8_t led;                 // Control del led de actividad
            float speed;                // Velocidad del cursor
            int32_t save_timer;         // Temporizador para el guardado
            bool save_flag;             // Flag de guardado
            int32_t restore_timer;      // Temporizador para la restauracion
            bool restore_flag;          // Flag para la restauracion
        } mouse;

        // Variables de control del puerto RS-232
        struct {
            bool rts;
        } rs232;

        // Metodo para establecer los valores por defecto
        void SetDefaults();
        // Metodo para configurar los PINS de la placa Arduino
        void SetPins();
        // Metodo para configurar el protocolo PS/2
        void SetPs2();
        // Metodo para configurar el protocolo SERIE de la placa Arduino
        void SetSerial();

        // Lectura de los datos del Raton PS/2
        void ReadMouse();
        // Analiza y prepara los datos para su envio
        void ParseData();
        // Led de actividad
        void ActivityLed();
        // Codifica el paquete de datos
        void EncodeData();
        // Envia los datos al puerto SERIE
        void SendData();
        // Monitor de la señal RTS del puerto serie
        void RtsStatusMonitor();

        // Guarda la configuracion de velocidad
        void SaveMouseSpeed();
        // Lee la configuracion de velocidad
        void LoadMouseSpeed();

};



#endif
