# PS2_Mouse_to_Serial_Port
Un convertidor para conectar un ratón PS/2 a un puerto serie (RS-232).


1. Introducción
----------------------------------------
Los ratones con conexión DB9 para puerto serie cada vez son más difíciles de encontrar (y más caros), lo que puede suponer un problema si necesitamos emplear uno en un ordenador clásico que solo disponga del puerto serie para su conexión. Después de investigar unas horas, encontré varias opciones comerciales y un par de proyectos open source que, usando placas Arduino, permitían la conexión de un ratón PS/2, o USB compatible con el protocolo PS/2 a un puerto serie.
https://github.com/Lameguy64/ps2serial
https://github.com/kristopher/PS2-Mouse-Arduino

Una vez documentado sobre estos proyectos y realizando un primer prototipo funcional de los mismos, decidí realizar mi propia versión del conversor.


2. Como funciona
----------------------------------------
Básicamente la placa Arduino se encarga de prácticamente todo el trabajo, necesitando solo un poco de electrónica externa para su funcionamiento. Una librería de interacción con el protocolo PS/2 no permite leer la señal codificada procedente del ratón con las entradas. Dicha información se codifica en un paquete de datos en el formato que esperan los drivers de ratón mas comunes y se envían al puerto serie. Además, se monitorea la señal RTS del puerto serie por si se solicita el reinicio del envío de datos.


3. Hardware usado en el prototipo
----------------------------------------
Se han realizado dos prototipos de prueba, usando las placas Arduino UNO R3 y Arduino NANO, funcionando ambas placas perfectamente. El resto de electrónica sirve para la adaptación de las señales que circulan por el puerto serie, ofreciendo cierto nivel de aislamiento y protección entre este puerto y la placa Arduino. Además, se incorporan dos diodos LED indicadores de estado, para el monitoreo del comportamiento del prototipo.


4. Que indican los LED’s de estado
----------------------------------------
En mi caso he usado un LED rojo para el indicador de alimentación y un LED amarillo para el indicador de estado. Dicho indicador nos proporciona cierta información en el momento del arranque del adaptador, así como de su funcionamiento.
Al encender el adaptador, si no puede detectar un ratón compatible conectado, el LED de estado quedará iluminado permanentemente. Si el proceso de inicio se realiza correctamente, este LED emitirá 3 parpadeos, uno largo seguido de 2 mas cortos. Una vez iniciado el adaptador, si movemos el ratón, observaremos que este LED parpadea regularmente, quedando encendido mientras mantengamos pulsado alguno de los 3 botones del ratón. Si se recibe desde el puerto serie la señal RTS, este LED emitirá un parpadeo de aproximadamente medio segundo. Esto nos permite verificar que el driver ha detectado correctamente nuestro adaptador.


5. Precauciones
----------------------------------------
- Conectar y desconectar siempre el ratón al puerto PS/2 con el adaptador desconectado. La conexión o desconexión del ratón con el adaptador conectado a la alimentación podría causar daños al ratón y al propio adaptador. Esto también se aplica a la conexión del adaptador con el puerto serie del ordenador, siendo muy recomendable que todas las conexiones se realicen con los equipos apagados.
- Cualquier dispositivo conectado al puerto RS-232 del ordenador puede causar daños a dicho puerto o al dispositivo si este tiene algún error de montaje, por lo que se recomienda que repaséis 3 veces las conexiones antes de conectarlo a vuestro ordenador.