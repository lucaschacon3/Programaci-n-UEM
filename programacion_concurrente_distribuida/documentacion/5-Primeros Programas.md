# Sesión 5 — Primer programa concurrente con Python

**Programación Concurrente · Alumnos / Proyección**

> ¿Cómo hacemos que varias peticiones avancen mientras esperan?

---

## Qué vamos a practicar

- Crear hilos con `threading.Thread`.
- Indicar función y argumentos con `target` y `args`.
- Iniciar con `start()` y esperar con `join()`.
- Comparar tiempos y observar el orden de ejecución.

---

## Del concepto al código

```text
Programa → Proceso Python
           ├── Hilo principal
           ├── Hilo A
           ├── Hilo B
           └── Hilo C
```

```python
import threading
```

---

## Escenario: tres peticiones

| Petición | Operación simulada | Espera |
|---|---|---:|
| A | Consulta a base de datos | 3 s |
| B | Llamada a API | 2 s |
| C | Acceso a almacenamiento | 1 s |

Cada petición inicia, espera y finaliza.

`time.sleep()` simula la espera: **no realiza una consulta real**.

---

## Primero: versión secuencial

```python
import time


def procesar_peticion(nombre, tiempo):
    print(f"{nombre}: iniciando")
    time.sleep(tiempo)
    print(f"{nombre}: finalizada")


inicio = time.time()
procesar_peticion("Petición A", 3)
procesar_peticion("Petición B", 2)
procesar_peticion("Petición C", 1)
print(f"Tiempo total: {time.time() - inicio:.2f} s")
```

---

## Una línea de ejecución

```text
A [──── 3 s ────] → B [── 2 s ──] → C [1 s]
Total aproximado: 6 s
```

> Si las peticiones son independientes y esperan, ¿por qué iniciar B solo después de terminar A?

---

## Crear un hilo: `Thread`

Con la función `procesar_peticion` anterior:

```python
import threading

hilo_a = threading.Thread(
    target=procesar_peticion,
    args=("Petición A", 3)
)
```

| Elemento | Función |
|---|---|
| `Thread` | Crea el objeto hilo |
| `target` | Referencia a la función que ejecutará |
| `args` | Tupla de argumentos para esa función |

---

## `target`: pasar una función

```python
target=procesar_peticion
```

La función se pasa **sin llamarla**.

```python
procesar_peticion("Petición A", 3)
```

Esto sí es una llamada: ejecuta la función en ese momento.

---

## Crear e iniciar son pasos distintos

```python
hilo_a.start()
```

```text
Thread(...) → objeto creado
start()     → inicia la ejecución en otro hilo
```

`start()` permite que el hilo principal continúe.

---

## Añadir varios hilos

```python
hilo_b = threading.Thread(
    target=procesar_peticion,
    args=("Petición B", 2)
)
hilo_c = threading.Thread(
    target=procesar_peticion,
    args=("Petición C", 1)
)

hilo_a.start()
hilo_b.start()
hilo_c.start()
```

Las tres peticiones pueden estar en progreso durante el mismo intervalo.

---

## El hilo principal también avanza

Fragmento para analizar, tras crear los hilos:

```python
inicio = time.time()
hilo_a.start()
hilo_b.start()
hilo_c.start()
print(f"Tiempo total: {time.time() - inicio:.2f} s")
```

> ¿Qué está midiendo? ¿Han terminado necesariamente las peticiones al imprimir?

---

## Esperar con `join()`

```python
hilo_a.join()
```

**El hilo que llama a `join()` espera a que termine `hilo_a`.**

```python
hilo_a.join()
hilo_b.join()
hilo_c.join()
print("Todas las peticiones han terminado")
```

Los tres hilos deben haberse iniciado antes de esta espera.

---

## Programa concurrente · 1/2

Ejemplo completo dividido en dos bloques consecutivos.

```python
import threading
import time


def procesar_peticion(nombre, tiempo):
    print(f"{nombre}: iniciando")
    time.sleep(tiempo)
    print(f"{nombre}: finalizada")


inicio = time.time()
hilo_a = threading.Thread(
    target=procesar_peticion, args=("Petición A", 3)
)
hilo_b = threading.Thread(
    target=procesar_peticion, args=("Petición B", 2)
)
hilo_c = threading.Thread(
    target=procesar_peticion, args=("Petición C", 1)
)
```

---

## Programa concurrente · 2/2

Continuación del bloque anterior:

```python
hilo_a.start()
hilo_b.start()
hilo_c.start()

hilo_a.join()
hilo_b.join()
hilo_c.join()

fin = time.time()
print("Todas las peticiones han terminado")
print(f"Tiempo total: {fin - inicio:.2f} segundos")
```

**Primero iniciar todos; después esperar a todos.**

---

## Comparación temporal

```text
Secuencial: A [3 s] → B [2 s] → C [1 s]   ≈ 6 s

Concurrente:
A [──── 3 s ────]
B [── 2 s ──]
C [1 s]                                  ≈ 3 s
```

Las barras incluyen esperas.

`sleep()` pausa el hilo que lo ejecuta; otros pueden avanzar.

La mejora de este ejemplo procede de **solapar esperas**, no de multiplicar la capacidad de cálculo.

---

## Actividad: compara estos fragmentos

**Caso A**

```python
a.start()
a.join()
b.start()
b.join()
```

**Caso B**

```python
a.start()
b.start()
a.join()
b.join()
```

Dibuja una línea temporal de cada caso. ¿Cuándo puede comenzar B?

---

## Orden no determinista

- El orden global de los mensajes puede variar.
- El orden de inicio no garantiza el orden de finalización.
- Cada hilo sigue su propia secuencia.
- Varias ejecuciones iguales no demuestran que exista un orden garantizado.

---

## Observar intercalados

```python
import threading


def mostrar(nombre):
    for i in range(5):
        print(nombre, i)


a = threading.Thread(target=mostrar, args=("A",))
b = threading.Thread(target=mostrar, args=("B",))
a.start()
b.start()
a.join()
b.join()
```

Ejecuta varias veces. ¿Qué orden se mantiene dentro de cada hilo?

---

## Una versión con listas

Con `procesar_peticion` e importaciones ya definidos:

```python
peticiones = [
    ("Petición A", 3),
    ("Petición B", 2),
    ("Petición C", 1)
]
hilos = []

for nombre, tiempo_espera in peticiones:
    hilo = threading.Thread(
        target=procesar_peticion,
        args=(nombre, tiempo_espera)
    )
    hilos.append(hilo)
```

---

## Iniciar y esperar con bucles

Continuación:

```python
inicio = time.time()

for hilo in hilos:
    hilo.start()

for hilo in hilos:
    hilo.join()

print(f"Tiempo total: {time.time() - inicio:.2f} s")
```

La lista evita repetir la gestión de cada hilo manualmente.

---

## Actividad: descargar tres archivos

Función base de una simulación:

```python
import time


def descargar_archivo(nombre):
    print(f"Descargando {nombre}")
    time.sleep(2)
    print(f"{nombre} descargado")
```

1. Crea un hilo para `datos1.csv`, `datos2.csv` y `datos3.csv`.
2. Inicia todos y espera su finalización.
3. Compara con una versión secuencial.

**Una tupla de un elemento lleva coma:** `args=("datos1.csv",)`.

---

## Actividad: detecta el error

```python
hilo = threading.Thread(
    target=descargar_archivo("datos1.csv")
)
```

- ¿Cuándo se está ejecutando `descargar_archivo`?
- ¿Qué debería recibir `target`?
- Reescribe la creación del hilo.

---

## Consultar varios servidores

Función base; añade los hilos para los servidores A, B, C y D:

```python
import random
import time


def consultar_servidor(nombre):
    print(f"Consultando {nombre}")
    tiempo = random.uniform(1, 3)
    time.sleep(tiempo)
    print(f"{nombre} respondió en {tiempo:.2f} s")
```

Es una simulación con esperas aleatorias.

---

## Actividad: registra cinco ejecuciones

| Ejecución | Primero | Segundo | Tercero | Cuarto |
|---|---|---|---|---|
| 1 | | | | |
| 2 | | | | |
| 3 | | | | |
| 4 | | | | |
| 5 | | | | |

¿Qué puede explicar las diferencias entre ejecuciones?

---

## Actividad final: monitor de servicios

Comprueba de forma simulada:

```text
Base de datos · API de pagos
Servidor de archivos · Servidor de autenticación
```

1. Define `comprobar_servicio(nombre)` con espera aleatoria de 1 a 4 s.
2. Crea un hilo por servicio e inicia todos.
3. Espera su finalización y mide el tiempo total.
4. Repite y registra el orden de finalización.
5. Compara con una versión secuencial.

---

## Resumen

| Elemento | Para qué sirve |
|---|---|
| `threading.Thread` | Crear un hilo |
| `target` | Elegir la función |
| `args` | Pasar argumentos |
| `start()` | Iniciar |
| `join()` | Esperar la finalización |
| `time.sleep()` | Simular espera |

**Siguiente sesión:** ¿qué ocurre si todos los hilos actualizan el mismo contador?
