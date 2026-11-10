<!-- PANTALLA 1 -->

# Sesión 1

## Abstracción, modularidad y Tipos Abstractos de Datos

Programación de Estructuras Lineales · Tema 1

---

<!-- PANTALLA 2 -->

# 1. Objetivos de la sesión

- Comprender **modularidad** y **abstracción**.
- Definir un **Tipo Abstracto de Datos (TAD)**.
- Identificar sus valores y operaciones.
- Separar el uso de un componente de su implementación.

---

<!-- PANTALLA 3 -->

# 2. El problema de la complejidad del software

> ¿Qué ocurre cuando un programa pasa de 200 a 200.000 líneas?

```text
Más código → más dependencias → cambios más difíciles
```

**Dividir el problema permite trabajar con partes manejables.**

---

<!-- PANTALLA 4 -->

# 3. Modularidad

**Dividir un sistema en componentes con una responsabilidad concreta.**

```text
Aplicación
├── Gestión de usuarios
├── Gestión de archivos
├── Base de datos
├── Comunicaciones
└── Interfaz gráfica
```

> En una aplicación como Spotify, ¿separarías reproducción y pagos?

---

<!-- PANTALLA 5 -->

# 4. Ejemplo informático: servidor web

```text
Servidor Web
├── Conexiones
├── Gestión HTTP
├── Autenticación
├── Logging
└── Acceso a base de datos
```

Cambio en `Logging`: **archivo → Elasticsearch**.

> ¿Qué tendría que conocer el resto del servidor para seguir funcionando?

---

<!-- PANTALLA 6 -->

# 5. Abstracción

**Utilizar una funcionalidad sin conocer todos sus detalles internos.**

```cpp
std::cout << "Hola";
```

```text
Programador → salida de información → terminal
```

> ¿Necesitamos conocer el controlador del dispositivo para usarla?

---

<!-- PANTALLA 7 -->

# 6. Otro ejemplo: abrir un archivo

```cpp
archivo.open("datos.txt");
```

```text
open()
├── Comprobar permisos
├── Localizar el archivo
├── Consultar el sistema de archivos
└── Preparar el acceso
```

Conocemos **qué operación usar** y **cómo invocarla**.

---

<!-- PANTALLA 8 -->

# 7. Abstracción: qué frente a cómo

| QUÉ hace | CÓMO lo hace |
|---|---|
| Interfaz | Implementación |
| `play()`, `pause()`, `stop()` | Buffers, hilos, decodificación |

> ¿Puede cambiar el decodificador sin cambiar las operaciones del reproductor?

---

<!-- PANTALLA 9 -->

# 8. Tipos Abstractos de Datos

Un **TAD** es un modelo formado por:

```text
TAD
├── Colección de valores
└── Operaciones permitidas sobre esos valores
```

**TAD = valores + operaciones**

La definición abstracta no fija cómo almacenar los datos.

---

<!-- PANTALLA 10 -->

# 9. Ejemplo: TAD Contador

Valores posibles: **0, 1, 2, 3, …**

```text
Contador
├── incrementar()
├── decrementar()
├── valor()
└── reiniciar()
```

> Si el contador solo admite valores no negativos, ¿qué debería pasar al decrementar 0?

---

<!-- PANTALLA 11 -->

# 10. Ejemplo informático: TAD Pila

```text
push("Chrome")
push("Visual Studio Code")
push("Terminal")
```

```text
┌────────────────────┐
│ Terminal           │ ← top()
│ Visual Studio Code │
│ Chrome             │
└────────────────────┘
```

**El último elemento insertado es el primero en salir.**

---

<!-- PANTALLA 12 -->

# 10. Pila: usar y representar

| Operación | Comportamiento |
|---|---|
| `push()` | Inserta en la cima |
| `pop()` | Elimina la cima |
| `top()` | Consulta la cima |
| `isEmpty()` | Comprueba si está vacía |

> Tras `pop()`, ¿qué aplicación queda en la cima?

Una pila puede implementarse con **array** o **memoria dinámica**.

---

<!-- PANTALLA 13 -->

# 11. Ejemplo: TAD Cola

```text
Entrada → [PDF 3] [PDF 2] [PDF 1] → Impresora
```

**El primero en entrar es el primero en salir.**

Operaciones: `enqueue()`, `dequeue()`, `front()`, `isEmpty()`.

> ¿Qué documento se imprimirá primero? ¿En qué se diferencia de una pila?

---

<!-- PANTALLA 14 -->

# 12. Ejemplo: sistema de archivos

```text
File
├── open()
├── close()
├── read()
├── write()
└── delete()
```

Uso conceptual: `file.open()` → `file.read()` → `file.close()`.

**Detalles ocultos:** bloques físicos, metadatos, cachés y descriptores.

---

<!-- PANTALLA 15 -->

# 13. Ejemplo: conexión de red

```text
Connection
├── connect()
├── send()
├── receive()
└── disconnect()
```

**Detalles internos posibles:** sockets, TCP, buffers, timeouts y retransmisiones.

> ¿Cuáles de esos detalles necesita gestionar el usuario del TAD?

---

<!-- PANTALLA 16 -->

# 14. ¿Por qué ocultar la implementación?

Cuenta bancaria simplificada: **saldo no negativo**.

```text
Usuario
   ↓
depositar() / retirar() / consultarSaldo()
   ↓
saldo interno
```

> ¿Cómo impedir que otra parte del programa escriba `saldo = -500000`?

Las operaciones pueden controlar qué cambios son válidos.

---

<!-- PANTALLA 17 -->

# 15. Diseño directo frente a abstracción

| Acceso a la representación | Acceso mediante operaciones |
|---|---|
| `array[]`, índice, capacidad | `push()`, `pop()`, `top()` |
| El cliente conoce detalles internos | El cliente conoce la interfaz |

> Si sustituimos el array por una lista enlazada, ¿qué diseño exige revisar más código?

---

<!-- PANTALLA 18 -->

# 16. Actividad: identificar TAD

| Tipo conceptual | Punto de partida |
|---|---|
| `BrowserTab` | Abrir y recargar una pestaña |
| `MediaPlayer` | Reproducir y controlar volumen |
| `User` | Iniciar sesión y cambiar contraseña |
| `DatabaseConnection` | Conectar y ejecutar consultas |

Para cada caso:

1. Propón operaciones públicas.
2. Identifica dos detalles que deberían permanecer ocultos.

---

<!-- PANTALLA 19 -->

# 17. Ejercicio guiado: TAD Printer

**Diseña conceptualmente una impresora gestionada por el sistema.**

1. ¿Qué representa el TAD?
2. ¿Qué operaciones debería ofrecer?
3. ¿Qué información podría ocultar?

> Piensa en imprimir, cancelar, pausar, reanudar y consultar el estado.

---

<!-- PANTALLA 20 -->

# 18. Pregunta conceptual

| Opción A | Opción B |
|---|---|
| `datos[]` | `push()` |
| `posicionActual` | `pop()` |
| `capacidad` | `top()`, `isEmpty()` |

En A, el usuario modifica directamente los campos.

> ¿Qué opción representa mejor la abstracción? Justifica tu respuesta.

---

<!-- PANTALLA 21 -->

# 19. Conexión con orientación a objetos

```text
TAD                      Clase C++
Valores / estado    →    Atributos
Operaciones         →    Métodos
```

**Una clase es un mecanismo para implementar un TAD.**

La sintaxis de C++ llegará en las sesiones 5 y 6.

---

<!-- PANTALLA 22 -->

# 20. Mini ejercicio final: TAD Download

Imagina el componente de descargas de un navegador.

Identifica:

1. Posibles estados de una descarga.
2. Operaciones para controlarla y consultar su progreso.
3. Información interna que el usuario no necesita conocer.

> ¿Qué operación tendría sentido cuando la descarga está pausada?

---

<!-- PANTALLA 23 -->

# 21. Cierre de la sesión

```text
Software complejo
   ↓
Modularidad + Abstracción
   ↓
TAD = valores + operaciones
   ↓
Definir QUÉ sin fijar CÓMO
```

> ¿Cómo describimos exactamente las operaciones de un TAD y su significado?

**Siguiente sesión:** especificación, interfaz pública, sintaxis y semántica.
