Este programa sirve para mandarle a los clientes datos de servidores spoofeados, con la idea de que arreglen esta vulnerabilidad
de la cual se están abusando FenixZone para ocultar a los demás servidores de las listas hosted y internet. Ver:

http://forum.sa-mp.com/showthread.php?t=588362<br>
http://forum.sa-mp.com/showthread.php?t=588429<br>
http://forum.sa-mp.com/showthread.php?t=596102<br>
http://forum.sa-mp.com/showthread.php?t=596134<br>

Demostración de cómo atacar con este programa: https://www.youtube.com/watch?v=JS5C49gIOuQ
  
Usa libcrafter (https://code.google.com/p/libcrafter) para componer los paquetes spoofeados y para escuchar los recibidos (el sniffer)

Funciona solamente en linux por la dependencia de libcrafter, pero puede ser portado a otras plataformas sin romper los códigos que usan las clases, lo único que hay que cambiar es la implementación (p. ejemplo, cambiar de libcrafter a libtins para hacerlo multiplataforma)

NOTA IMPORTANTE: Ejecutar como root (comando sudo en ubuntu) porque sino el Sniffer no va a poder captar paquetes.
