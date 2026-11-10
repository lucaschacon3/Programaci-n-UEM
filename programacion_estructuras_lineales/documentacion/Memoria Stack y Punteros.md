

---

# SESIÓN 1 — MEMORIA DE UN PROCESO, PILA Y FUNDAMENTOS DE PUNTEROS

**Programación con estructuras lineales**  
**Unidad: Memoria dinámica**

---

## 1. Objetivos de la sesión

Al terminar la sesión deberíamos ser capaces de:

- comprender cómo se organiza la memoria de un proceso;
- diferenciar memoria virtual y memoria física;
- entender qué es la pila del usuario;
- comprender qué es un `stack frame`;
- identificar errores habituales relacionados con la pila;
- comprender qué es un puntero;
- utilizar correctamente:
  - `&`
  - `*`
  - `->`
- trabajar con punteros a estructuras;
- diferenciar punteros constantes y punteros a objetos constantes.

---

# 2. ¿Dónde se guarda una variable?

Partimos de:

```cpp
int edad = 20;
```

### Pregunta

**¿Dónde está almacenado realmente el valor `20`?**

Un programa en ejecución necesita memoria para almacenar:

- código;
- variables;
- información de funciones;
- objetos;
- bibliotecas;
- otros datos necesarios durante la ejecución.

---

# 3. Espacio de memoria de un proceso

Podemos imaginar de forma simplificada:

```text
+-----------------------------+
|            KERNEL           |
+-----------------------------+
|         USER STACK          |
|                             |
+-----------------------------+
|      MEMORY MAPPING         |
+-----------------------------+
|         FREE STORE          |
|                             |
+-----------------------------+
| .bss                        |
| .data                       |
| .text                       |
+-----------------------------+
```

El material distingue estas regiones dentro del espacio de memoria virtual del proceso. 9750001108_UA2_T01

---

# 4. Principales zonas de memoria

## `.text`

Contiene el **código ejecutable** del programa.

Ejemplo:

```cpp
int suma(int a, int b)
{
    return a + b;
}
```

---

## `.data`

Variables estáticas o globales inicializadas.

```cpp
int contador = 10;
```

---

## `.bss`

Variables estáticas sin inicialización explícita.

```cpp
int contador;
```

---

## `free store`

Memoria que podrá solicitarse durante la ejecución.

Más adelante utilizaremos:

```cpp
new
```

y:

```cpp
delete
```

---

## `memory mapping`

Región utilizada, entre otros fines, por bibliotecas compartidas.

---

## `user stack`

La **pila del usuario**.

Se utiliza durante la ejecución de funciones y está muy relacionada con las variables locales.

---

# 5. Dos zonas especialmente importantes

```text
STACK
```

y

```text
FREE STORE
```

Comparación inicial:

| STACK | FREE STORE |
|---|---|
| Variables locales | Objetos dinámicos |
| Gestión ordenada | Gestión dinámica |
| LIFO | No sigue LIFO |
| Vida ligada normalmente al ámbito | Vida controlada explícitamente |
| Tamaño limitado | Tamaño mucho mayor |

La memoria libre será el tema principal de la segunda sesión.

---

# 6. Ejemplo

```cpp
int global = 4;

void ejemplo()
{
    int local = 5;
}
```

### Pregunta

¿Esperamos que `global` y `local` estén almacenadas exactamente de la misma manera?

**No.**

Tienen diferentes tipos de almacenamiento.

---

# 7. Memoria virtual

Un programa no trabaja directamente con la memoria física como si conociera exactamente la posición de cada dato en los chips de RAM.

Trabaja con un:

# **espacio de direcciones virtuales**

El sistema operativo y el hardware realizan la correspondencia con memoria física.

El documento describe este espacio virtual como asociado a cada proceso y gestionado por el sistema operativo junto con la MMU de la CPU. 9750001108_UA2_T01

---

# 8. Idea fundamental

```text
PROGRAMA
    ↓
direcciones virtuales
    ↓
Sistema operativo + hardware
    ↓
memoria física
```

---

# 9. Ejemplo conceptual

Dos procesos pueden tener:

```text
PROCESO A                 PROCESO B

0x0000                    0x0000
   |                         |
   | espacio virtual         | espacio virtual
   |                         |
0xFFFF                    0xFFFF
```

Cada proceso trabaja con su propio espacio virtual.

---

# 10. Paginación

La memoria virtual se divide en:

# **páginas virtuales**

La memoria física se divide en:

# **marcos de página**

El sistema operativo relaciona unas con otros. 9750001108_UA2_T01

---

# 11. Ejemplo de paginación

Memoria virtual:

```text
Página 0
Página 1
Página 2
Página 3
```

Memoria física:

```text
Marco 0
Marco 1
Marco 2
Marco 3
Marco 4
Marco 5
Marco 6
```

Una posible relación:

```text
Página 0 ─────→ Marco 5
Página 1 ─────→ Marco 2
Página 2 ─────→ Marco 6
Página 3 ─────→ Marco 1
```

---

# 12. Idea importante

Dos páginas consecutivas en memoria virtual:

```text
Página 0
Página 1
Página 2
```

no tienen por qué estar físicamente juntas en RAM.

---

# 13. Pregunta

Si dos direcciones son consecutivas desde el punto de vista del programa:

**¿significa necesariamente que están físicamente juntas en RAM?**

### No necesariamente.

---

# 14. Pila del usuario

La pila sigue el modelo:

# **LIFO**

**Last In, First Out**

```text
        ┌─────┐
        │  C  │ ← último en entrar
        ├─────┤
        │  B  │
        ├─────┤
        │  A  │
        └─────┘
```

Orden de salida:

```text
C
B
A
```

---

# 15. La pila y las funciones

Ejemplo:

```cpp
#include <iostream>

void g()
{
    int y = 20;
    std::cout << y << '\n';
}

void f()
{
    int x = 10;
    g();
}

int main()
{
    f();
}
```

---

# 16. Paso 1

Comienza:

```cpp
main()
```

```text
STACK

┌─────────────┐
│ main()      │
└─────────────┘
```

---

# 17. Paso 2

`main()` llama a:

```cpp
f();
```

```text
STACK

┌─────────────┐
│ f()         │
├─────────────┤
│ main()      │
└─────────────┘
```

---

# 18. Variable local de `f()`

```cpp
int x = 10;
```

```text
┌────────────────┐
│ f()            │
│ x = 10         │
├────────────────┤
│ main()         │
└────────────────┘
```

---

# 19. `f()` llama a `g()`

```cpp
g();
```

```text
┌────────────────┐
│ g()            │
├────────────────┤
│ f()            │
│ x = 10         │
├────────────────┤
│ main()         │
└────────────────┘
```

---

# 20. Variable local de `g()`

```cpp
int y = 20;
```

```text
┌────────────────┐
│ g()            │
│ y = 20         │
├────────────────┤
│ f()            │
│ x = 10         │
├────────────────┤
│ main()         │
└────────────────┘
```

---

# 21. Stack frame

Cada llamada a una función puede generar un:

# **stack frame**

Puede contener información como:

- variables locales;
- argumentos;
- registros;
- dirección de retorno.

El documento explica que al invocar una función se introduce normalmente un nuevo marco de pila y que este desaparece al finalizar la función. 9750001108_UA2_T01

---

# 22. Finaliza `g()`

Antes:

```text
┌───────────────┐
│ g()           │
├───────────────┤
│ f()           │
├───────────────┤
│ main()        │
└───────────────┘
```

Después:

```text
┌───────────────┐
│ f()           │
├───────────────┤
│ main()        │
└───────────────┘
```

---

# 23. Finaliza `f()`

```text
┌───────────────┐
│ main()        │
└───────────────┘
```

Se cumple:

# **Last In, First Out**

---

# 24. Variables locales

Ejemplo:

```cpp
void funcion()
{
    int x = 10;
}
```

Cuando termina:

```cpp
funcion()
```

la variable:

```cpp
x
```

deja de existir automáticamente.

No necesitamos liberarla manualmente.

---

# 25. ¿Por qué es eficiente la pila?

Su funcionamiento es:

- ordenado;
- predecible;
- muy rápido;
- basado en LIFO.

Además, la reutilización continua de esta región favorece el uso eficiente de la caché de la CPU. 9750001108_UA2_T01

---

# 26. Stack pointer

En x86-64 existe un registro especial llamado:

```text
%rsp
```

Es el:

# **stack pointer**

Referencia el extremo activo de la pila. 9750001108_UA2_T01

---

# 27. Tamaño limitado

La pila no tiene tamaño ilimitado.

El material utiliza como valor habitual en Linux:

```text
8 MiB
```

9750001108_UA2_T01

Lo importante:

# **la pila tiene un tamaño limitado**

---

# 28. Error 1 — Stack overflow

Se produce cuando agotamos el espacio disponible de la pila.

Posibles causas:

- variables locales muy grandes;
- recursión demasiado profunda;
- recursión infinita.

9750001108_UA2_T01

---

# 29. Ejemplo: variable muy grande

```cpp
void funcion()
{
    int datos[10'000'000];
}
```

Si un `int` ocupa aproximadamente 4 bytes:

```text
10.000.000 × 4
≈ 40 MB
```

Puede superar ampliamente la capacidad disponible de la pila.

---

# 30. Ejemplo: recursión infinita

```cpp
void hola()
{
    hola();
}
```

Se generan llamadas continuamente:

```text
hola()
hola()
hola()
hola()
hola()
...
```

La pila sigue creciendo.

Resultado posible:

```text
STACK OVERFLOW
```

---

# 31. Recursión correcta

```cpp
int factorial(int n)
{
    if (n == 0)
        return 1;

    return n * factorial(n - 1);
}
```

Para:

```cpp
factorial(5)
```

llamadas:

```text
factorial(5)
factorial(4)
factorial(3)
factorial(2)
factorial(1)
factorial(0)
```

Después regresan en orden inverso.

---

# 32. Error 2 — Acceso fuera de límites

```cpp
int numeros[4] = {10, 20, 30, 40};
```

Son válidos:

```cpp
numeros[0]
numeros[1]
numeros[2]
numeros[3]
```

Pero:

```cpp
numeros[7]
```

no corresponde a ningún elemento válido.

---

# 33. ¿Por qué es peligroso?

Representación conceptual:

```text
┌──────────────┐
│ numeros[0]   │
├──────────────┤
│ numeros[1]   │
├──────────────┤
│ numeros[2]   │
├──────────────┤
│ numeros[3]   │
├──────────────┤
│ otra cosa    │
├──────────────┤
│ otra cosa    │
└──────────────┘
```

Un acceso fuera del array puede afectar a memoria que no pertenece al array.

---

# 34. `std::array`

Ejemplo del tema:

```cpp
std::array<int, 4> m = {1, 2, 3, 4};

int a = m[3];

int b = m.at(5);
```

`at()` realiza control de límites y puede generar:

```cpp
std::out_of_range
```

9750001108_UA2_T01

---

# 35. Comprobación rápida

```cpp
std::array<int, 5> datos = {2, 4, 6, 8, 10};
```

¿Qué devuelve?

```cpp
datos[2];
```

### Resultado

```text
6
```

---

¿Qué devuelve?

```cpp
datos.at(2);
```

### Resultado

```text
6
```

---

¿Qué ocurre con?

```cpp
datos.at(7);
```

### Resultado

```text
std::out_of_range
```

---

# 36. Punteros

Definición fundamental:

# **Un puntero es una variable que almacena una dirección de memoria.**

9750001108_UA2_T01

---

# 37. Una variable normal

```cpp
int n = 10;
```

Podemos imaginar:

```text
Dirección       Contenido

0x1000          10
                 ↑
                 n
```

Toda variable ocupa alguna posición en memoria.

---

# 38. Obtener la dirección

Operador:

```cpp
&
```

Ejemplo:

```cpp
&n
```

significa:

# **dirección de `n`**

---

# 39. Ejemplo

```cpp
#include <iostream>

int main()
{
    int n = 10;

    std::cout << n << '\n';
    std::cout << &n << '\n';
}
```

Posible salida:

```text
10
0x7ffee21ad90c
```

---

# 40. Regla

```text
n    → valor de n

&n   → dirección de n
```

---

# 41. Crear un puntero

```cpp
int n = 10;

int* p = &n;
```

Lectura:

> `p` es un puntero a `int` y almacena la dirección de `n`.

---

# 42. Representación

```text
p
┌────────────┐
│ 0x1000     │
└────────────┘
      │
      │
      ▼
n
┌────────────┐
│ 10         │
└────────────┘

dirección de n:
0x1000
```

---

# 43. Idea fundamental

```text
p
```

NO contiene:

```text
10
```

Contiene:

```text
la dirección de n
```

---

# 44. ¿Qué imprime `p`?

```cpp
std::cout << p;
```

Imprime la dirección almacenada.

Conceptualmente:

```cpp
p == &n
```

---

# 45. Desreferenciar un puntero

Operador:

```cpp
*
```

Si:

```cpp
int n = 10;
int* p = &n;
```

entonces:

```cpp
*p
```

significa:

# **el objeto apuntado por `p`**

---

# 46. Ejemplo

```cpp
std::cout << *p;
```

Resultado:

```text
10
```

El documento diferencia explícitamente `p` —dirección— y `*p` —valor almacenado en el objeto apuntado—. 9750001108_UA2_T01

---

# 47. Regla fundamental

```text
p    → dirección almacenada

*p   → objeto situado en esa dirección

&p   → dirección del propio puntero
```

---

# 48. El puntero también es una variable

Supongamos:

```cpp
int n = 10;
int* p = &n;
```

Podríamos imaginar:

```text
Dirección       Contenido

0x1000          10
                ↑
                n


0x2000          0x1000
                ↑
                p
```

---

# 49. Relaciones

```text
n       = 10

&n      = 0x1000

p       = 0x1000

&p      = 0x2000

*p      = 10
```

---

# 50. Modificar mediante un puntero

```cpp
int n = 10;

int* p = &n;

*p = 25;

std::cout << n;
```

Resultado:

```text
25
```

---

# 51. ¿Por qué cambia `n`?

Antes:

```text
p ─────────► n
             10
```

Ejecutamos:

```cpp
*p = 25;
```

Después:

```text
p ─────────► n
             25
```

---

# 52. Ejemplo completo

```cpp
#include <iostream>

int main()
{
    int edad = 20;

    int* pEdad = &edad;

    std::cout << "Valor de edad: "
              << edad << '\n';

    std::cout << "Direccion de edad: "
              << &edad << '\n';

    std::cout << "Valor de pEdad: "
              << pEdad << '\n';

    std::cout << "Valor apuntado: "
              << *pEdad << '\n';

    *pEdad = 21;

    std::cout << "Nueva edad: "
              << edad << '\n';
}
```

### Pregunta

¿Qué mostrará finalmente?

```text
Nueva edad: 21
```

---

# 53. Atención al símbolo `*`

En:

```cpp
int* p;
```

significa:

> `p` es un puntero a `int`.

Pero en:

```cpp
*p
```

significa:

> acceder al objeto apuntado.

---

# 54. Tamaño de un puntero

El tamaño de un puntero depende del tamaño necesario para almacenar una dirección.

El documento indica:

```text
x86     → 4 bytes
x86-64  → 8 bytes
```

9750001108_UA2_T01

---

# 55. Ejemplo

```cpp
int* pi;
double* pd;
char* pc;
```

En una arquitectura x86-64 típica:

```text
sizeof(pi)
sizeof(pd)
sizeof(pc)
```

serán normalmente:

```text
8 bytes
```

aunque los objetos apuntados tengan tamaños diferentes.

---

# 56. Pregunta conceptual

```cpp
double temperatura = 36.5;

double* p = &temperatura;
```

¿Qué representa cada expresión?

```text
temperatura
&temperatura
p
*p
&p
```

---

# 57. Solución

```text
temperatura
→ 36.5

&temperatura
→ dirección de temperatura

p
→ dirección de temperatura

*p
→ 36.5

&p
→ dirección del propio puntero
```

---

# 58. Punteros y `const`

Dos construcciones diferentes:

```cpp
const int* p;
```

y:

```cpp
int* const p = ...;
```

No significan lo mismo. 9750001108_UA2_T01

---

# 59. Puntero a objeto constante

```cpp
int n = 10;

const int* p = &n;
```

También:

```cpp
int const* p = &n;
```

Podemos leer:

```cpp
std::cout << *p;
```

Pero no:

```cpp
*p = 20;
```

---

# 60. ¿Puede cambiar `p`?

Sí.

```cpp
int n = 10;
int m = 30;

const int* p = &n;

p = &m;
```

Es válido.

---

# 61. Idea

```cpp
const int* p;
```

significa:

```text
puedo cambiar el puntero
```

pero:

```text
no puedo modificar el objeto
a través de ese puntero
```

---

# 62. Puntero constante

```cpp
int n = 10;

int* const p = &n;
```

Ahora:

```text
p
```

no puede cambiar de dirección.

---

# 63. No permitido

```cpp
int m = 20;

p = &m;
```

No es válido porque:

```cpp
p
```

es constante.

---

# 64. Sí permitido

```cpp
*p = 25;
```

El objeto apuntado sí puede modificarse.

---

# 65. Comparación

| Declaración | ¿Puede cambiar `p`? | ¿Puede cambiar `*p`? |
|---|---:|---:|
| `int* p` | Sí | Sí |
| `const int* p` | Sí | No |
| `int* const p` | No | Sí |
| `const int* const p` | No | No |

---

# 66. Punteros a estructuras

Ejemplo:

```cpp
struct Student
{
    std::string name;

    double grade1;
    double grade2;
    double grade3;

    double average() const
    {
        return (grade1 + grade2 + grade3) / 3.0;
    }
};
```

El documento utiliza una estructura equivalente para introducir punteros a objetos. 9750001108_UA2_T01

---

# 67. Crear un estudiante

```cpp
Student s{
    "James Kerry",
    6.0,
    8.0,
    9.5
};
```

Acceso normal:

```cpp
s.name

s.grade2

s.average()
```

---

# 68. Puntero al estudiante

```cpp
Student* p = &s;
```

Representación:

```text
p
│
│
▼
┌─────────────────────┐
│ Student s           │
├─────────────────────┤
│ name                │
│ grade1              │
│ grade2              │
│ grade3              │
└─────────────────────┘
```

---

# 69. Acceso mediante desreferencia

```cpp
(*p).grade2 = 8.5;
```

Primero:

```cpp
*p
```

obtenemos el objeto.

Después:

```cpp
.grade2
```

accedemos a uno de sus miembros.

---

# 70. Operador flecha

C++ proporciona:

```cpp
->
```

Por tanto:

```cpp
p->grade2 = 8.5;
```

equivale a:

```cpp
(*p).grade2 = 8.5;
```

---

# 71. También con funciones miembro

```cpp
p->average();
```

equivale a:

```cpp
(*p).average();
```

---

# 72. Regla práctica

Objeto:

```cpp
objeto.miembro
```

Puntero:

```cpp
puntero->miembro
```

---

# 73. Ejemplo

```cpp
Student s;

Student* p = &s;

s.grade1 = 10;

p->grade1 = 10;
```

Si `p` apunta a `s`, ambas expresiones modifican el mismo miembro.

---

# 74. Ejemplo completo

```cpp
#include <iostream>
#include <string>

struct Student
{
    std::string name;
    double grade1;
    double grade2;
    double grade3;

    double average() const
    {
        return (grade1 + grade2 + grade3) / 3.0;
    }
};

int main()
{
    Student s{
        "Ana",
        7.0,
        8.0,
        9.0
    };

    Student* p = &s;

    std::cout << p->name << '\n';

    std::cout << p->average() << '\n';

    p->grade1 = 10.0;

    std::cout << s.grade1 << '\n';
}
```

---

# 75. Pregunta

¿Qué imprime finalmente?

```cpp
std::cout << s.grade1;
```

### Resultado

```text
10
```

Porque:

```cpp
p
```

apunta a:

```cpp
s
```

---

# 76. Actividad

Analiza sin ejecutar:

```cpp
#include <iostream>

struct Point
{
    int x;
    int y;
};

int main()
{
    Point a{3, 5};

    Point* p = &a;

    p->x = 10;

    (*p).y = 20;

    std::cout << a.x << " "
              << a.y << '\n';
}
```

### ¿Qué imprimirá?

---

# 77. Solución

```text
10 20
```

Porque:

```cpp
p->x
```

equivale a:

```cpp
(*p).x
```

y ambos accesos modifican el objeto:

```cpp
a
```

---

# 78. Confusión habitual 1

Si:

```cpp
int n = 10;
int* p = &n;
```

Incorrecto:

```text
p contiene 10
```

Correcto:

```text
p contiene la dirección de n

*p vale 10
```

---

# 79. Confusión habitual 2

```cpp
int* p;
```

y:

```cpp
*p
```

no representan lo mismo.

```cpp
int* p;
```

declara un puntero.

```cpp
*p
```

desreferencia el puntero.

---

# 80. Confusión habitual 3

El puntero también tiene dirección.

```text
n
dirección → 0x1000
valor     → 10

p
dirección → 0x2000
valor     → 0x1000
```

---

# 81. Confusión habitual 4

Tener un puntero:

```cpp
int n = 10;

int* p = &n;
```

NO significa necesariamente que estemos utilizando memoria dinámica.

Aquí no aparece:

```cpp
new
```

---

# 82. Confusión habitual 5

No confundir:

```cpp
const int* p;
```

con:

```cpp
int* const p;
```

Primero:

```text
objeto protegido mediante p
```

Segundo:

```text
puntero que no puede cambiar de dirección
```

---

# 83. Resumen de la sesión

```text
                   PROCESO
                      │
        ┌─────────────┴─────────────┐
        │                           │
      STACK                     FREE STORE
        │                           │
        │                           │
 llamadas a funciones        memoria dinámica
 variables locales           próxima sesión
 stack frames
        │
        │
      PUNTEROS
        │
        ├── &  dirección
        │
        ├── *  desreferencia
        │
        └── -> miembro mediante puntero
```

---

# 84. Lo esencial sobre punteros

```cpp
int n = 10;

int* p = &n;
```

Debemos saber responder:

```text
¿Qué es n?
¿Qué es &n?
¿Qué es p?
¿Qué es *p?
¿Qué es &p?
```

---

# 85. Solución

```text
n
→ el entero, cuyo valor es 10

&n
→ dirección de n

p
→ puntero que almacena la dirección de n

*p
→ objeto apuntado por p: n

&p
→ dirección del propio puntero
```

---

# 86. Repaso rápido

### 1

¿Qué diferencia existe entre memoria virtual y física?

---

### 2

¿Por qué las llamadas a funciones encajan con una estructura LIFO?

---

### 3

¿Qué puede producir un `stack overflow`?

---

### 4

Dado:

```cpp
int x = 50;
int* p = &x;
```

¿Qué ocurre con?

```cpp
*p = 75;
```

---

### 5

Dado:

```cpp
Student s;
Student* p = &s;
```

¿Qué diferencia existe entre:

```cpp
s.grade1
```

y:

```cpp
p->grade1
```

---

# 87. Ejercicio final

```cpp
#include <iostream>
#include <string>

struct Book
{
    std::string title;
    int pages;
};

int main()
{
    int n = 5;

    int* p = &n;

    Book book{
        "C++ Fundamentals",
        300
    };

    Book* pBook = &book;

    *p = 8;

    pBook->pages = 450;

    std::cout << n << '\n';

    std::cout << book.pages << '\n';
}
```

---

# 88. Preguntas

1. ¿Qué contiene inicialmente `p`?
2. ¿Qué representa `*p`?
3. ¿Qué hace `*p = 8`?
4. ¿A qué objeto apunta `pBook`?
5. ¿Qué significa `pBook->pages`?
6. ¿Qué imprime el programa?

---

# 89. Solución

```text
1. p contiene la dirección de n.

2. *p representa n.

3. Cambia n de 5 a 8.

4. pBook apunta al objeto book.

5. Accede al miembro pages del objeto apuntado.

6. El programa imprime:

8
450
```

---

# 90. Próxima sesión

Hasta ahora hemos trabajado con:

```cpp
int n = 10;

int* p = &n;
```

El puntero apunta a una variable que ya existe.

Pero:

### ¿Podemos solicitar memoria nueva durante la ejecución?

### ¿Qué ocurre si no conocemos de antemano el número de elementos que necesitaremos?

### ¿Qué ocurre si necesitamos que un objeto tenga un tiempo de vida diferente al de una variable local?

El punto de partida será:

```cpp
Student* p = new Student;
```

Y estudiaremos:

```text
nullptr

new

new[]

delete

delete[]

memory leaks

fragmentación

aritmética de punteros
```

La unidad contrapone precisamente la pila, utilizada para ejecución de funciones y variables locales, con la memoria libre, utilizada para objetos y estructuras dinámicas. 9750001108_UA2_T01

Esta versión queda **sincronizada punto por punto con la versión del profesor**: cuando el profesor explique un concepto, el alumno tendrá proyectado el esquema, ejemplo o pregunta correspondiente, sin saturar la pantalla con la explicación oral completa.