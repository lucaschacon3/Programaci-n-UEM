

---

# Sesión 1 — Introducción a la programación genérica y TAD genéricos

## 1. Objetivos

Al finalizar la sesión deberíamos ser capaces de comprender:

- por qué aparece la programación genérica;
- qué es un **tipo parametrizado**;
- qué representa `T`;
- qué significa `Lista<T>`;
- qué es una **familia de tipos**;
- cómo la genericidad evita duplicar código;
- por qué será útil para vectores, listas, pilas y colas.

El tema introduce los TAD genéricos como base para las estructuras de datos que se desarrollarán posteriormente. 

---

# 2. El problema: repetir código

Queremos almacenar distintos tipos:

```cpp
int procesos[5];

double tiempos[5];

std::string servidores[5];
```

La estructura es similar.

Lo que cambia es:

```text
int
double
string
```

---

# 3. Una posible solución...

```cpp
class IntArray {
    int data_[10];
};
```

```cpp
class DoubleArray {
    double data_[10];
};
```

```cpp
class StringArray {
    std::string data_[10];
};
```

### Pregunta

> ¿Estamos repitiendo la misma idea?

---

# 4. ¿Qué cambia realmente?

```text
ESTRUCTURA
──────────
igual


OPERACIONES
───────────
iguales


TIPO DE DATO
────────────
cambia
```

---

# 5. La idea de programación genérica

En lugar de:

```text
ListaInt
ListaDouble
ListaString
```

queremos:

```text
Lista<T>
```

---

# 6. ¿Qué representa `T`?

```text
T = un tipo todavía no especificado
```

Posteriormente:

```text
T = int
```

o:

```text
T = double
```

o:

```text
T = string
```

o:

```text
T = Process
```

---

# 7. Una definición, muchos tipos

```text
                    Lista<T>
                       │
          ┌────────────┼────────────┐
          ↓            ↓            ↓
   Lista<int>   Lista<string>   Lista<Process>
```

---

# 8. TAD genérico

```text
TAD GENÉRICO
=
TAD
+
PARÁMETROS
```

Un TAD genérico permite crear distintas especializaciones según los parámetros proporcionados. 

---

# 9. Familia de tipos

`Lista<T>` representa una:

```text
FAMILIA DE TIPOS
```

Todos comparten:

```text
las mismas operaciones
```

pero cambia:

```text
el tipo almacenado
```

---

# 10. Ejemplo: lista de procesos

```text
Lista<Process>
```

Podría almacenar:

```text
Process 101
Process 102
Process 103
```

Operaciones:

```text
insert()
remove()
find()
size()
```

---

# 11. Ejemplo: lista de conexiones

```text
Lista<Connection>
```

Puede almacenar:

```text
Connection A
Connection B
Connection C
```

Y seguir ofreciendo:

```text
insert()
remove()
find()
size()
```

---

# 12. La estructura permanece

Podemos tener:

```text
Lista<int>
Lista<string>
Lista<Process>
Lista<Connection>
```

pero todas siguen siendo:

```text
LISTAS
```

---

# 13. Ejemplo: pila genérica

En lugar de:

```text
StackInt
StackProcess
StackCommand
```

podemos pensar en:

```text
Stack<T>
```

---

# 14. Especializaciones de `Stack<T>`

```text
Stack<int>
```

```text
Stack<Process>
```

```text
Stack<Command>
```

Todas pueden ofrecer:

```text
push()
pop()
top()
empty()
```

---

# 15. Pila de procesos

```text
Stack<Process>

┌─────────────┐
│ Process 203 │ ← top
├─────────────┤
│ Process 155 │
├─────────────┤
│ Process 101 │
└─────────────┘
```

---

# 16. Cola genérica

Podemos definir:

```text
Queue<T>
```

y utilizar:

```text
Queue<Process>
Queue<NetworkPacket>
Queue<PrintJob>
```

La lógica de la cola permanece.

---

# 17. Programación genérica y contenedores

Una aplicación habitual son los:

```text
CONTENEDORES
```

Ejemplos:

```text
array
vector
lista
pila
cola
```

El documento señala especialmente el uso de TAD genéricos para estructuras de datos como vectores y listas. 

---

# 18. Genericidad y abstracción

Antes abstraíamos:

```text
CÓMO está implementada una estructura
```

Ahora también abstraemos:

```text
QUÉ TIPO almacena
```

---

# 19. Sin genericidad

```text
ProcessQueue

PacketQueue

JobQueue
```

Tres implementaciones.

---

# 20. Con genericidad

```text
Queue<T>
```

y utilizamos:

```text
Queue<Process>
Queue<Packet>
Queue<Job>
```

---

# 21. Beneficios

```text
MENOS DUPLICACIÓN
        ↓
MENOS CÓDIGO
        ↓
MÁS REUTILIZACIÓN
        ↓
MANTENIMIENTO MÁS SENCILLO
```

---

# 22. Ejemplo: corregir un error

Sin genericidad:

```text
ProcessQueue
PacketQueue
JobQueue
```

Puede ser necesario corregir varias implementaciones.

Con:

```text
Queue<T>
```

la lógica común se mantiene en un único diseño.

---

# 23. Primer contacto con `Array<T,N>`

El ejemplo principal del tema será:

```text
Array<T,N>
```

donde:

```text
T → tipo de dato
N → longitud
```

El documento lo presenta como una familia biparamétrica de arrays. 

---

# 24. Ejemplos de `Array<T,N>`

```text
Array<int,5>
```

```text
5 enteros
```

---

```text
Array<double,10>
```

```text
10 doubles
```

---

```text
Array<Server,3>
```

```text
3 servidores
```

---

# 25. Dos parámetros distintos

En:

```text
Array<T,N>
```

tenemos:

```text
T
↓
parámetro de tipo
```

y:

```text
N
↓
parámetro de valor
```

---

# 26. Familia biparamétrica

```text
                     Array<T,N>
                         │
         ┌───────────────┼────────────────┐
         ↓               ↓                ↓
 Array<int,5>   Array<double,10>   Array<Server,3>
```

---

# 27. Ejemplo informático: buffer

Podríamos imaginar:

```text
Buffer<T,N>
```

y utilizar:

```text
Buffer<Packet,64>
```

o:

```text
Buffer<LogEntry,1000>
```

¿Qué cambia?

```text
T
N
```

¿Qué permanece?

```text
la lógica del buffer
```

---

# 28. De TAD genérico a C++

Conceptualmente:

```text
TAD GENÉRICO
      ↓
Array<T,N>
```

En C++ utilizaremos:

```text
PLANTILLAS DE CLASE
```

El documento indica que C++ implementa los TAD genéricos mediante `class templates`. 

---

# 29. Primer vistazo

Más adelante veremos código como:

```cpp
template<typename T>
class Container {
    // ...
};
```

La idea:

```text
T será sustituido
por un tipo concreto
```

---

# 30. Ejemplo

```cpp
Container<int>
```

```text
T = int
```

---

```cpp
Container<double>
```

```text
T = double
```

---

# 31. El compilador

Conceptualmente:

```text
PLANTILLA
   ↓
tipo solicitado
   ↓
versión concreta
```

Ejemplo:

```text
Array<T,N>
+
T = int
N = 4

        ↓

Array<int,4>
```

---

# 32. Actividad rápida

Tenemos:

```text
Buffer<NetworkPacket,64>
```

### ¿Qué representa `T`?

```text
NetworkPacket
```

### ¿Qué representa `N`?

```text
64
```

---

# 33. Actividad

Tenemos:

```text
Queue<Process>
Queue<Packet>
Queue<Job>
```

### ¿Qué parte hemos parametrizado?

```text
el tipo almacenado
```

---

# 34. Detecta la duplicación

```cpp
class IntBuffer { ... };

class DoubleBuffer { ... };

class StringBuffer { ... };
```

### Pregunta

> ¿Cómo podríamos representar una única solución?

```text
Buffer<T>
```

---

# 35. ¿Qué NO significa genericidad?

No significa:

```text
"un contenedor puede guardar
cualquier tipo mezclado"
```

Por ejemplo:

```text
List<int>
```

almacena:

```text
int
```

---

# 36. `T` no es un valor

Correcto:

```text
T = int
```

Incorrecto:

```text
T = 42
```

`T` representa:

```text
UN TIPO
```

---

# 37. `N` sí representa un valor

En:

```text
Array<int,5>
```

tenemos:

```text
T = int
N = 5
```

---

# 38. Las estructuras que veremos más adelante

La programación genérica será especialmente útil para:

```text
Vector<T>
List<T>
Stack<T>
Queue<T>
```

El material conecta explícitamente este tema con vectores, listas, pilas y colas. 

---

# 39. Resumen

```text
PROGRAMACIÓN GENÉRICA
        │
        ↓
parametrizar aquello que cambia
        │
        ↓
       TAD
        │
        ↓
   TAD GENÉRICO
        │
        ↓
     Lista<T>
     Stack<T>
     Queue<T>
   Array<T,N>
```

---

# 40. Idea clave

> **La programación genérica permite escribir una solución general y utilizarla posteriormente con diferentes tipos.**

```text
ESCRIBIR UNA VEZ
       +
PARAMETRIZAR EL TIPO
       =
REUTILIZAR
```

---

# 41. Preguntas de cierre

```text
¿Qué representa T?
```

```text
¿Qué diferencia existe entre Lista<int> y Lista<string>?
```

```text
¿Por qué no crear una clase distinta para cada tipo?
```

```text
¿Qué representan T y N en Array<T,N>?
```

---

# 42. Próxima sesión

## Sesión 2 — Especificación de `Array<T,N>`

Analizaremos:

```text
Array<T,N> TYPE

T → tipo
N → longitud
```

y sus operaciones:

```text
init
at
front
back
size
empty
```

para comprobar cómo una **única especificación algebraica puede describir toda una familia de tipos genéricos**. 