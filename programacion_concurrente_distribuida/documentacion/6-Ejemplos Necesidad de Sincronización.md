# Sesión 1 — Necesidad de sincronización y comunicación

## Ejemplos para clase

### Objetivo de la sesión

En esta sesión **no se estudian todavía mecanismos** como `Lock`, `Semaphore`, `Queue` o `Barrier`.

El objetivo es que el alumno **vea el problema antes de conocer la solución**:

```text
EJECUTAR
    ↓
OBSERVAR
    ↓
IDENTIFICAR EL PROBLEMA
    ↓
PREGUNTAR QUÉ GARANTÍA FALTA
```

Los tres ejemplos comparten un mismo contexto: **un servidor que atiende peticiones**. Cada uno muestra un tipo de problema distinto.

| Ejemplo | Problema | Conceptos |
|---|---|---|
| 1 | Acceso concurrente a datos compartidos | Hilos independientes, recurso compartido, condición de carrera, lectura/escritura |
| 2 | Recurso de capacidad limitada | Gestión de recursos, "hasta N" frente a "solo 1" |
| 3 | Productor y consumidor sin coordinación | Comunicación frente a sincronización, espera, señal de fin |

> **Nota sobre los `time.sleep()`:** se usan para que los problemas aparezcan **siempre** en clase. Sin ellos, los mismos errores existen igualmente, pero se manifiestan de forma esporádica y difícil de reproducir.

---

# Ejemplo 1 — Estadísticas de un servidor web

Se presenta en **tres programas cortos** que se ejecutan en orden. Los tres hacen algo parecido; lo que cambia es **qué datos comparten los hilos**.

```text
1A  Cada hilo con sus datos       →  sin problema
1B  Todos escriben el mismo dato  →  actualización perdida
1C  Uno escribe, otro lee         →  estado inconsistente
```

---

## Programa 1A — Cada worker con su propio contador

### Objetivo

Mostrar que tener varios hilos **no implica automáticamente un problema**.

### Código

```python
import threading
import time


def atender_peticiones(num_peticiones):
    nombre = threading.current_thread().name

    # Variable LOCAL: cada hilo tiene la suya
    atendidas = 0

    for _ in range(num_peticiones):
        time.sleep(0.2)          # simula el trabajo de una petición
        atendidas += 1

    print(f"{nombre}: {atendidas} peticiones atendidas")


hilos = []

for i in range(3):
    hilo = threading.Thread(
        target=atender_peticiones,
        args=(5,),
        name=f"Worker-{i}"
    )
    hilos.append(hilo)
    hilo.start()

for hilo in hilos:
    hilo.join()

print("\nCada worker ha llevado su propia cuenta sin interferencias.")
```

### Salida obtenida

```text
Worker-0: 5 peticiones atendidas
Worker-2: 5 peticiones atendidas
Worker-1: 5 peticiones atendidas

Cada worker ha llevado su propia cuenta sin interferencias.
```

> El orden de las líneas puede cambiar de una ejecución a otra, pero **los números son siempre correctos**.

### Qué debe observar el alumno

- La variable `atendidas` es **local**: cada hilo tiene su propia copia.
- Ningún hilo puede ver ni modificar el contador de otro.
- No hay nada que proteger.



```text
CONCURRENCIA  ≠  PROBLEMA AUTOMÁTICO
```

---

## Programa 1B — Un contador compartido

### Objetivo

Hacer visible una **condición de carrera**: varios hilos modifican el mismo dato y se pierden actualizaciones.

### Código

```python
import threading
import time

# Variable GLOBAL: compartida por todos los hilos
contador_peticiones = 0


def atender_peticion():
    global contador_peticiones
    nombre = threading.current_thread().name

    # 1. LEER
    valor_leido = contador_peticiones
    print(f"{nombre} lee contador = {valor_leido}")

    # Simula el trabajo de la petición (y provoca un cambio de hilo)
    time.sleep(0.5)

    # 2. MODIFICAR
    nuevo_valor = valor_leido + 1

    # 3. ESCRIBIR
    contador_peticiones = nuevo_valor
    print(f"{nombre} escribe contador = {nuevo_valor}")


hilos = []

for i in range(5):
    hilo = threading.Thread(target=atender_peticion, name=f"Worker-{i}")
    hilos.append(hilo)
    hilo.start()

for hilo in hilos:
    hilo.join()

print("\nPeticiones atendidas: 5")
print("Valor del contador:  ", contador_peticiones)
```

### Salida obtenida

```text
Worker-0 lee contador = 0
Worker-1 lee contador = 0
Worker-2 lee contador = 0
Worker-3 lee contador = 0
Worker-4 lee contador = 0
Worker-3 escribe contador = 1
Worker-1 escribe contador = 1
Worker-0 escribe contador = 1
Worker-2 escribe contador = 1
Worker-4 escribe contador = 1

Peticiones atendidas: 5
Valor del contador:   1
```

### Qué debe observar el alumno

Se han atendido **5 peticiones**, pero el contador vale **1**.

La operación "incrementar" son en realidad **tres pasos**:

```text
LEER  →  MODIFICAR  →  ESCRIBIR
```

y los pasos de los distintos hilos se **intercalan**:

```text
Worker-0        Worker-1        Worker-2   ...

lee 0
                lee 0
                                lee 0
calcula 1
                calcula 1
                                calcula 1
escribe 1
                escribe 1
                                escribe 1
```

Cada hilo sobrescribe el trabajo de los anteriores:

```text
ACTUALIZACIÓN PERDIDA
```


---

## Programa 1C — Un dashboard que lee mientras otro hilo escribe

### Objetivo

Mostrar que el problema no requiere dos escritores: basta con **uno que escribe y otro que lee**.

### Código

```python
import threading
import time

# Estado compartido: dos datos que deben ser coherentes entre sí
estado_servidor = {
    "peticiones": 100,
    "errores": 5
}


def registrar_peticion_fallida():
    print("Worker: registrando una petición fallida...")

    estado_servidor["peticiones"] += 1     # paso 1 de la actualización

    time.sleep(1)                          # algo retrasa al worker

    estado_servidor["errores"] += 1        # paso 2 de la actualización

    print("Worker: registro completado.")


def mostrar_dashboard():
    time.sleep(0.5)                        # el dashboard consulta "en mal momento"

    peticiones = estado_servidor["peticiones"]
    errores = estado_servidor["errores"]

    print("\n--- DASHBOARD ---")
    print("Peticiones:", peticiones)
    print("Errores:   ", errores)
    print(f"Tasa de error: {errores / peticiones:.2%}\n")


hilo_worker = threading.Thread(target=registrar_peticion_fallida)
hilo_dashboard = threading.Thread(target=mostrar_dashboard)

hilo_worker.start()
hilo_dashboard.start()

hilo_worker.join()
hilo_dashboard.join()

print("Estado final:", estado_servidor)
print(f"Tasa de error real: "
      f"{estado_servidor['errores'] / estado_servidor['peticiones']:.2%}")
```

### Salida obtenida

```text
Worker: registrando una petición fallida...

--- DASHBOARD ---
Peticiones: 101
Errores:    5
Tasa de error: 4.95%

Worker: registro completado.
Estado final: {'peticiones': 101, 'errores': 6}
Tasa de error real: 5.94%
```

### Qué debe observar el alumno

Registrar una petición fallida exige **dos cambios** que deben ir juntos:

```text
peticiones + 1
errores    + 1
```

El dashboard lee **entre los dos cambios** y ve una combinación que nunca debería existir para un observador externo:

| | Peticiones | Errores | Tasa de error |
|---|---:|---:|---:|
| Antes | 100 | 5 | 5,00 % |
| **Lo que ve el dashboard** | **101** | **5** | **4,95 %** |
| Después | 101 | 6 | 5,94 % |

El dashboard informa de que la tasa de error **ha bajado**, cuando en realidad **ha subido**.



---

# Ejemplo 2 — Pool de conexiones a base de datos

### Objetivo

Presentar un problema **distinto** al del ejemplo 1: no se trata de que un dato se corrompa, sino de que **un recurso tiene capacidad limitada**.

```text
8 workers  ──►  3 conexiones disponibles
```

### Código

```python
import threading
import time
import random

CONEXIONES_MAXIMAS = 3

conexiones_en_uso = 0
maximo_simultaneo = 0

inicio = time.time()


def instante():
    return f"[t={time.time() - inicio:4.1f}s]"


def consultar_bd():
    global conexiones_en_uso, maximo_simultaneo
    nombre = threading.current_thread().name

    # Ocupa una conexión (sin comprobar si queda alguna libre)
    conexiones_en_uso += 1
    maximo_simultaneo = max(maximo_simultaneo, conexiones_en_uso)

    aviso = "  <-- CAPACIDAD SUPERADA" if conexiones_en_uso > CONEXIONES_MAXIMAS else ""
    print(f"{instante()} {nombre} entra   | en uso: {conexiones_en_uso}{aviso}")

    # Simula la consulta
    time.sleep(random.uniform(1, 2))

    # Libera la conexión
    conexiones_en_uso -= 1
    print(f"{instante()} {nombre} sale    | en uso: {conexiones_en_uso}")


hilos = []

for i in range(8):
    hilo = threading.Thread(target=consultar_bd, name=f"Worker-{i}")
    hilos.append(hilo)
    hilo.start()

for hilo in hilos:
    hilo.join()

print(f"\nLímite de conexiones:         {CONEXIONES_MAXIMAS}")
print(f"Máximo simultáneo alcanzado:  {maximo_simultaneo}")
```

### Salida obtenida

```text
[t= 0.0s] Worker-0 entra   | en uso: 1
[t= 0.0s] Worker-1 entra   | en uso: 2
[t= 0.0s] Worker-2 entra   | en uso: 3
[t= 0.0s] Worker-3 entra   | en uso: 4  <-- CAPACIDAD SUPERADA
[t= 0.0s] Worker-4 entra   | en uso: 5  <-- CAPACIDAD SUPERADA
[t= 0.0s] Worker-5 entra   | en uso: 6  <-- CAPACIDAD SUPERADA
[t= 0.0s] Worker-6 entra   | en uso: 7  <-- CAPACIDAD SUPERADA
[t= 0.0s] Worker-7 entra   | en uso: 8  <-- CAPACIDAD SUPERADA
[t= 1.0s] Worker-2 sale    | en uso: 7
[t= 1.1s] Worker-7 sale    | en uso: 6
[t= 1.4s] Worker-1 sale    | en uso: 5
[t= 1.6s] Worker-4 sale    | en uso: 4
[t= 1.6s] Worker-5 sale    | en uso: 3
[t= 1.8s] Worker-0 sale    | en uso: 2
[t= 1.9s] Worker-3 sale    | en uso: 1
[t= 1.9s] Worker-6 sale    | en uso: 0

Límite de conexiones:         3
Máximo simultáneo alcanzado:  8
```

> Los instantes de salida cambian en cada ejecución (la duración de cada consulta es aleatoria), pero **siempre se superan las 3 conexiones**.

### Qué debe observar el alumno

- Los 8 workers entran a la vez.
- Se llegan a usar **8 conexiones** cuando solo existen **3**.
- En un sistema real, los workers 3 a 7 recibirían un error de conexión o bloquearían la base de datos.

### Diferencia con el ejemplo 1

Aquí el problema **no es aleatorio**: aparece en todas las ejecuciones. No se debe a que los hilos se intercalen mal, sino a que **no existe ningún mecanismo que haga esperar** a quien no tiene sitio.


---

# Ejemplo 3 — Cola de pedidos: productor y consumidor

### Objetivo

Introducir la diferencia entre:

```text
COMUNICACIÓN    →  ¿cómo pasa un dato de un hilo a otro?
SINCRONIZACIÓN  →  ¿cómo sabe un hilo CUÁNDO puede usarlo?
```

mediante el patrón:

```text
PRODUCTOR  ──►  BUFFER (lista)  ──►  CONSUMIDOR
```

### Código

```python
import threading
import time
import random

NUM_PEDIDOS = 6

pedidos = []            # buffer compartido entre productor y consumidor
resumen = None          # resultado que calculará el consumidor

consultas_vacias = 0
procesados = 0


def productor():
    for i in range(NUM_PEDIDOS):
        pedido = f"Pedido-{i}"
        pedidos.append(pedido)
        print(f"Productor: recibe {pedido}")

        time.sleep(random.uniform(0.5, 1.0))   # los pedidos llegan a ritmo irregular

    print("Productor: no hay más pedidos.")


def consumidor():
    global consultas_vacias, procesados, resumen

    for _ in range(6):                         # ¿cuántas veces debe mirar?
        if len(pedidos) > 0:
            pedido = pedidos.pop(0)
            procesados += 1
            print(f"    Consumidor: procesa {pedido}")
        else:
            consultas_vacias += 1
            print("    Consumidor: ¿hay pedidos? ... no")

        time.sleep(0.4)

    resumen = f"{procesados} pedidos procesados"
    print("    Consumidor: termina.")


def informe():
    print(f"        Informe: {resumen}")


hilo_productor = threading.Thread(target=productor)
hilo_consumidor = threading.Thread(target=consumidor)
hilo_informe = threading.Thread(target=informe)

hilo_productor.start()
hilo_consumidor.start()
hilo_informe.start()

hilo_productor.join()
hilo_consumidor.join()
hilo_informe.join()

print("\n--- BALANCE ---")
print("Pedidos recibidos:     ", NUM_PEDIDOS)
print("Pedidos procesados:    ", procesados)
print("Pedidos sin procesar:  ", pedidos)
print("Consultas en vacío:    ", consultas_vacias)
```

### Salida obtenida

```text
Productor: recibe Pedido-0
    Consumidor: procesa Pedido-0
        Informe: None
    Consumidor: ¿hay pedidos? ... no
Productor: recibe Pedido-1
    Consumidor: procesa Pedido-1
    Consumidor: ¿hay pedidos? ... no
Productor: recibe Pedido-2
    Consumidor: procesa Pedido-2
    Consumidor: ¿hay pedidos? ... no
Productor: recibe Pedido-3
    Consumidor: termina.
Productor: recibe Pedido-4
Productor: recibe Pedido-5
Productor: no hay más pedidos.

--- BALANCE ---
Pedidos recibidos:      6
Pedidos procesados:     3
Pedidos sin procesar:   ['Pedido-3', 'Pedido-4', 'Pedido-5']
Consultas en vacío:     3
```

> Los números exactos varían entre ejecuciones, pero **siempre quedan pedidos sin procesar** y **siempre hay consultas en vacío**.

### Qué debe observar el alumno

El programa tiene **tres fallos visibles**:

#### 1. Espera activa

```text
Consumidor: ¿hay pedidos? ... no
Consumidor: ¿hay pedidos? ... no
```

El consumidor gasta su tiempo preguntando. Lo razonable sería que **esperara** hasta que llegue un pedido.

#### 2. Pedidos perdidos

El consumidor termina tras 6 consultas porque **no tiene forma de saber cuándo ha terminado el productor**. Los pedidos que llegan después se quedan en la lista sin atender.

Falta una **señal de fin**.

#### 3. Un resultado leído antes de estar listo

```text
Informe: None
```

El hilo `informe` lee `resumen` antes de que el consumidor lo haya calculado. El dato **llega** (está en una variable compartida), pero nadie le dice al informe **cuándo** está disponible.

### Comunicación y sincronización son problemas distintos

| Pregunta | En este programa | ¿Funciona? |
|---|---|---|
| ¿Cómo pasa el pedido del productor al consumidor? | A través de la lista `pedidos` | Sí |
| ¿Cómo sabe el consumidor que hay un pedido nuevo? | Preguntando continuamente | Mal |
| ¿Cómo sabe el consumidor que no habrá más pedidos? | No lo sabe | No |
| ¿Cómo pasa el resumen al hilo `informe`? | Variable compartida `resumen` | Sí |
| ¿Cómo sabe `informe` que el resumen está listo? | No lo sabe | No |

```text
Tener un canal para el dato (COMUNICACIÓN)
no resuelve cuándo usarlo (SINCRONIZACIÓN)
```



# Actividad de cierre

A partir de los programas anteriores, el alumno completa esta tabla **para cada ejemplo** (1B, 1C, 2 y 3):

| Elemento | Análisis |
|---|---|
| Unidades concurrentes | |
| Recursos compartidos | |
| Hilos lectores | |
| Hilos escritores | |
| Recursos con capacidad limitada | |
| Posibles condiciones de carrera | |
| Necesidades de comunicación | |
| Necesidades de sincronización | |

Y después clasifica cada ejemplo como:

```text
SOLO SINCRONIZACIÓN
SINCRONIZACIÓN + COMUNICACIÓN
```

justificando la respuesta.

> **No se pide todavía implementar ninguna solución.** El objetivo es detectar qué garantía falta en cada caso.

---

