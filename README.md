# PS2_Mouse_to_Serial_Port
Un convertidor para conectar un ratón PS/2 a un puerto serie (RS-232).

Con la escasez (y precio) que actualmente sufren los ratones de puerto serie (DB9) y en base a estos proyectos:

	https://github.com/Lameguy64/ps2serial
	https://github.com/kristopher/PS2-Mouse-Arduino
	
decidí crear mi propia versión de un adaptador de PS/2 a puerto serie, para así poder usar cualquier ratón que use este conector (y seguramente los USB compatibles con protocolo PS/2) en ordenadores antiguos que solo disponen del puerto serie (DB9 o DB25).

El proyecto se basa en el uso de una placa Arduino y un mínimo de electrónica externa para realizar la adaptación de un protocolo al otro.

Actualmente, el proyecto aún está en fase de desarrollo, pero el prototipo está funcionando perfectamente en un ordenador Amstrad PC1512 bajo el sistema MS-DOS 6.22, usando como driver el CuteMouse 1.9, habiéndose probado con éxito en programas como Deluxe Paint, Paint Brush, Microsoft Works, Norton Commander, Edit, QuickBasic, etc.

Recordar que cualquier interfaz conectada al puerto RS-232 es delicada y un error puede causar daños tanto al dispositivo como al propio puerto del ordenador, por lo que el uso de los datos de este proyecto están bajo la responsabilidad del usuario.