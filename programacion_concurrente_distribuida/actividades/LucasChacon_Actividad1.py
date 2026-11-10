"""


Escribir el código de un programa "Carreras de las tortugas", el cual es un simulador de una carrera de tortugas 
en el que varias "tortugas" se mueven de manera aleatoria hasta que una de ellas alcanza la meta. 
Aquí hay un resumen de lo que ocurriría:

1. Al ejecutar el programa, se crearán múltiples hilos asociados a las tortugas en la forma que se desee.
2. En cada iteración de mover_tortuga(), cada tortuga generará una velocidad aleatoria y avanzará una distancia aleatoria. 
Esto simula el progreso de cada tortuga en la carrera.
3. El programa mostrará el progreso de cada tortuga en la consola, indicando cuántos metros ha recorrido cada una.
4. Las tortugas continuarán moviéndose de manera aleatoria hasta que una de ellas alcance o supere los 100 metros, 
que es la distancia de la meta.
5. Una vez que una tortuga haya cruzado la meta, el programa mostrará un mensaje indicando que esa tortuga ha ganado la carrera.
6. Finalmente, el programa esperará a que todos los hilos hayan terminado de ejecutarse antes de finalizar.
El resultado específico puede variar en cada ejecución debido a la aleatoriedad en la velocidad y el progreso de las tortugas. 
Puede que una tortuga gane en una ejecución y otra en la siguiente.


"""