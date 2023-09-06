/*******************************************************************************
  
    Adaptador de mouse PS2 a puerto serie RS-232
	
	--- Manual del usuario ---
	
	
    Proyecto iniciado el Martes, 25 de Julio del 2023
    por Cesar Rincon "NightFox"

    https://nightfoxandco.com
    contact@nightfoxandco.com

    "Adaptador de mouse PS2 a puerto serie RS-232" is under MIT License
    Copyright (c) 2023 by Cesar Rincon "NightFox"

*******************************************************************************/


1. Introducción
--------------------------------------------------------------------------------
Los ratones con conexión DB9 para puerto serie cada vez son más difíciles
de encontrar (y más caros), lo que puede suponer un problema si necesitamos
emplear uno en un ordenador clásico que solo disponga del puerto serie para
su conexión. Después de investigar unas horas, encontré varias opciones
comerciales y un par de proyectos open source que, usando placas Arduino,
permitían la conexión de un ratón PS/2, o USB compatible con el protocolo
PS/2 a un puerto serie.

https://github.com/Lameguy64/ps2serial
https://github.com/kristopher/PS2-Mouse-Arduino

Una vez documentado sobre estos proyectos y
realizando un primer prototipo funcional de los mismos, decidí realizar mi
propia versión del conversor.


2. Como funciona
--------------------------------------------------------------------------------
Básicamente la placa Arduino se encarga de prácticamente todo el trabajo,
necesitando solo un poco de electrónica externa para su funcionamiento.
Una librería de interacción con el protocolo PS/2 no permite leer la señal
codificada procedente del ratón con las entradas. Dicha información se
codifica en un paquete de datos en el formato que esperan los drivers de
ratón mas comunes y se envían al puerto serie. Además, se monitorea la
señal RTS del puerto serie por si se solicita el reinicio del envío de datos.


3. Hardware usado en el prototipo
--------------------------------------------------------------------------------
Se han realizado dos prototipos de prueba, usando las placas Arduino UNO R3
y Arduino NANO, funcionando ambas placas perfectamente. El resto de
electrónica sirve para la adaptación de las señales que circulan por el
puerto serie, ofreciendo cierto nivel de aislamiento y protección entre este
puerto y la placa Arduino. Además, se incorporan dos diodos LED indicadores
de estado, para el monitoreo del comportamiento del prototipo.


4. Que indican los LED’s de estado
--------------------------------------------------------------------------------
En mi caso he usado un LED rojo para el indicador de alimentación y un LED
amarillo para el indicador de estado. Dicho indicador nos proporciona cierta
información en el momento del arranque del adaptador, así como de su
funcionamiento. Al encender el adaptador, si no puede detectar un ratón
compatible conectado, quedará encendido de manera permanente. Dicho LED de
estado parpadeará de manera intermitente una vez por segundo si ha detectado un
ratón, pero no es posible iniciar la comunicación siguiendo el protocolo PS/2.
Si el proceso de inicio se realiza correctamente, este LED emitirá 3
parpadeos, uno largo seguido de 2 mas cortos. Una vez iniciado el adaptador,
si movemos el ratón, observaremos que este LED parpadea regularmente,
quedando encendido mientras mantengamos pulsado alguno de los 3 botones del
ratón. Si movemos la rueda de scroll, tambien emitira un parpadeo.
Si se recibe desde el puerto serie la señal RTS, este LED emitirá un
parpadeo de aproximadamente medio segundo. Esto nos permite verificar que el
driver ha detectado correctamente nuestro adaptador.


5. Capacidades extendidas
--------------------------------------------------------------------------------
Dado que este proyecto (en especial la placa ISA) está destinado a ordenadores
sin puerto PS/2 (normalmente 486 y anteriores), usando el sistema operativo
MS-DOS o Windows 3.11 y anteriores, junto con el hecho de que actualmente casi
todos los ratones PS/2 son ópticos y disponen de la rueda de scroll, sumándole
que para estos ordenadores casi ningún programa hace uso de la rueda de scroll,
he decidido usar dicha rueda para ajustar a tiempo real la velocidad de
desplazamiento del cursor. Dado que, según la resolución de pantalla, programa o
modelo de ratón esta puede ser diferente, creo que es una característica útil.
La velocidad seleccionada se conservará hasta que apaguemos el adaptador (o el
ordenador en el caso de la placa ISA), no viendose afectada por el reinicio
del sistema. Si queremos guardar la velocidad actual de manera permanente,
deberemos de mantener pulsados los botones izquierdo y derecho de manera
simultanea durante 5 segundos, observando que el LED de estado se apaga
transcurrido ese tiempo. Esto nos indica que la configuración de ha almacenado
de manera correcta.
Si en algún momento deseamos restaurar la configuración por defecto, deberemos
de mantener presionados ambos botones durante 10 segundos, el LED de estado se
apagará a los 5 segundos, volviéndose a encender transcurridos 10 segundos.
Esto indicará que la configuración se ha restaurado y almacenado con éxito.


6. Precauciones
--------------------------------------------------------------------------------
- Conectar y desconectar siempre el ratón al puerto PS/2 con el adaptador
desconectado. La conexión o desconexión del ratón con el adaptador conectado
a la alimentación podría causar daños al ratón y al propio adaptador. Esto
también se aplica a la conexión del adaptador con el puerto serie del ordenador,
siendo muy recomendable que todas las conexiones se realicen con los
equipos apagados.
- Cualquier dispositivo conectado al puerto RS-232 del ordenador puede causar
daños a dicho puerto o al dispositivo si este tiene algún error de montaje,
por lo que se recomienda que repaséis 3 veces las conexiones antes de conectarlo
a vuestro ordenador.
- Aplicable a la versión ISA, en ningún caso actualicéis el firmware del modulo
Arduino con la tarjeta ISA insertada en la placa base. Retirar la tarjeta ISA
o el módulo Arduino antes de conectarlo al puerto USB para su actualización.
De no hacerlo, se corre el riesgo de dañar el módulo Arduino o la propia placa
base del ordenador.
