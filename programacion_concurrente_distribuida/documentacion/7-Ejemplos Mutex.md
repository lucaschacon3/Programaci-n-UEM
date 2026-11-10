# Sesión 2 — Condiciones de carrera y exclusión mutua

## Ejemplos para clase

### De dónde venimos

En la Sesión 1 vimos el problema sin resolverlo. En el programa **1B** varios workers incrementaban un contador compartido y se **perdían actualizaciones**: 5 peticiones atendidas, contador = 1.

Esta sesión continúa justo ahí:

```text
SESIÓN 1   "Tenemos un problema."
    ↓
SESIÓN 2   "Entendemos por qué ocurre."
    ↓       "Localizamos la sección crítica."
    ↓       "Aplicamos exclusión mutua."
    ↓       "Aprendemos a usar bien el Lock."
```

### Objetivos de la sesión

Al terminar, el alumno debe ser capaz de:

1. Localizar la **sección crítica** de un programa: la parte que no puede ejecutarse por varios hilos a la vez.
2. Proteger esa sección con un **`Lock`** (mutex), primero con `acquire()`/`release()` y después con `with`.
3. Explicar por qué **`with` es más seguro** que `acquire()`/`release()`.
4. Distinguir **`start()`**, **`join()`** y **`Lock`**: cada uno resuelve un problema distinto.
5. Reconocer los **tres errores más frecuentes** al usar un lock.
6. Aplicar todo ello a un caso más realista que un simple contador.

### Estructura

| Ejemplo | Programas | Idea central |
|---|---|---|
| 1 — Del problema a la solución | 1A, 1B, 1C | Sección crítica, `Lock`, `with` |
| 2 — Usar bien el lock | 2A, 2B, 2C | Tres errores frecuentes |
| 3 — Caso integrador | 3A (ejercicio), 3B (solución) | Aplicar todo a un servidor |

> **Nota sobre los `time.sleep()`:** igual que en la Sesión 1, se usan para que los problemas aparezcan **en todas las ejecuciones**. Sin ellos, los errores siguen existiendo, pero aparecen de forma esporádica. Eso es todavía peor en un programa real, porque pueden pasar las pruebas y fallar en producción.

---

# Ejemplo 1 — Del problema a la solución: el contador

Partimos del programa que falló en la Sesión 1 y lo corregimos en dos pasos.

```text
1A  Programa sin proteger    →  localizar la sección crítica
1B  acquire() / release()    →  la exclusión mutua funciona
1C  with                     →  la exclusión mutua es segura ante errores
```

---

## Programa 1A — Localizar la sección crítica

### Objetivo

Retomar el programa de la Sesión 1 con dos preguntas nuevas:

- ¿**Qué líneas exactas** no pueden ejecutarse por varios hilos a la vez?
- El programa ya usa `join()`: **¿por qué no basta?**

### Código

```python
# Programa 1A — El contador de la Sesión 1, revisado
# --------------------------------------------------
# Es el mismo programa 1B de la Sesión 1. No se corrige todavía:
# sólo se marca dónde está la SECCIÓN CRÍTICA y se observa
# que join() no evita el problema.

import threading
import time

# RECURSO COMPARTIDO: todos los hilos leen y escriben esta variable
contador_peticiones = 0


def atender_peticion():
    global contador_peticiones
    nombre = threading.current_thread().name

    # ===== INICIO DE LA SECCIÓN CRÍTICA =====
    # Desde que se LEE el contador hasta que se ESCRIBE el nuevo valor,
    # ningún otro hilo debería poder tocarlo.

    valor_leido = contador_peticiones          # 1. LEER
    print(f"{nombre} lee contador = {valor_leido}")

    time.sleep(0.5)                            # trabajo que fuerza el cambio de hilo

    nuevo_valor = valor_leido + 1              # 2. MODIFICAR
    contador_peticiones = nuevo_valor          # 3. ESCRIBIR
    print(f"{nombre} escribe contador = {nuevo_valor}")

    # ===== FIN DE LA SECCIÓN CRÍTICA =====


hilos = []

for i in range(5):
    hilo = threading.Thread(target=atender_peticion, name=f"Worker-{i}")
    hilos.append(hilo)
    hilo.start()          # start(): el hilo empieza a ejecutarse en paralelo

# join(): el hilo principal ESPERA a que cada worker termine.
# Sólo garantiza que los resultados están completos al imprimir;
# NO impide que los workers se pisen entre sí mientras se ejecutan.
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
Worker-0 escribe contador = 1
Worker-1 escribe contador = 1
Worker-2 escribe contador = 1
Worker-3 escribe contador = 1
Worker-4 escribe contador = 1

Peticiones atendidas: 5
Valor del contador:   1
```

### Explicación

#### ¿Qué es la sección crítica?

Es el **fragmento de código que accede a un recurso compartido** y que, para dar un resultado correcto, debe ejecutarse por **un solo hilo cada vez**.

Aquí son las líneas que van desde **leer** el contador hasta **escribir** el nuevo valor:

```python
valor_leido = contador_peticiones      # LEER
...
nuevo_valor = valor_leido + 1          # MODIFICAR
contador_peticiones = nuevo_valor      # ESCRIBIR
```

**Justificación:** si un hilo lee el contador y, antes de escribir, otro hilo lee el mismo valor, los dos escribirán el mismo resultado y uno de los incrementos se perderá. Por eso **las tres operaciones forman una unidad lógica**: no basta con proteger una de ellas, como se verá en el programa 2A.

#### ¿Por qué no basta `join()`?

El programa ya llama a `join()` y aun así falla. Cada mecanismo resuelve un problema diferente:

| Mecanismo | Qué hace | Qué problema resuelve |
|---|---|---|
| `start()` | Pone el hilo en ejecución | Crear concurrencia |
| `join()` | El hilo que lo llama **espera a que otro termine** | Saber que un hilo **ha acabado** |
| `Lock` | Sólo un hilo puede tenerlo a la vez | Que dos hilos **no accedan a la vez** a un recurso |

`join()` se ejecuta en el **hilo principal** y sólo actúa **al final**. Mientras los workers se ejecutan, nada les impide pisarse. Sin `join()`, el `print` final podría mostrar un valor todavía incompleto. Con `join()`, muestra el valor final, **que ya es incorrecto**.

### Actividad en papel

Construir dos intercalados posibles de dos workers (A y B) partiendo de `contador = 10`:

| Intercalado que da **12** (correcto) | Intercalado que da **11** (incorrecto) |
|---|---|
| A lee 10 | A lee 10 |
| A escribe 11 | B lee 10 |
| B lee 11 | A escribe 11 |
| B escribe 12 | B escribe 11 |

**Conclusión:** el resultado depende del **intercalado**, y el programador no controla el intercalado. Por tanto, hay que **impedir** los intercalados peligrosos.

---

## Programa 1B — Exclusión mutua con `acquire()` y `release()`

### Objetivo

Introducir el **`Lock`** (también llamado *mutex*, de *mutual exclusion*) y comprobar que resuelve el problema.

### Código

```python
# Programa 1B — Exclusión mutua con acquire() y release()
# -------------------------------------------------------
# Se añade un Lock (mutex) que rodea la sección crítica.
# Sólo un hilo puede tenerlo a la vez: los demás esperan.

import threading
import time

contador_peticiones = 0

# Un ÚNICO lock, compartido por todos los hilos, protege el contador
mutex = threading.Lock()

inicio = time.perf_counter()


def instante():
    """Tiempo transcurrido desde el inicio, para ver quién espera a quién."""
    return f"[t={time.perf_counter() - inicio:3.1f}s]"


def atender_peticion():
    global contador_peticiones
    nombre = threading.current_thread().name

    print(f"{instante()} {nombre} quiere entrar")

    mutex.acquire()                # ADQUIRIR: si otro hilo lo tiene, se queda aquí esperando

    # ===== SECCIÓN CRÍTICA (sólo un hilo cada vez) =====
    valor_leido = contador_peticiones
    print(f"{instante()} {nombre} ENTRA y lee {valor_leido}")

    time.sleep(0.5)

    contador_peticiones = valor_leido + 1
    print(f"{instante()} {nombre} escribe {contador_peticiones} y SALE")
    # ===== FIN DE LA SECCIÓN CRÍTICA =====

    mutex.release()                # LIBERAR: deja pasar al siguiente hilo que espera


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
[t=0.0s] Worker-0 quiere entrar
[t=0.0s] Worker-0 ENTRA y lee 0
[t=0.0s] Worker-1 quiere entrar
[t=0.0s] Worker-2 quiere entrar
[t=0.0s] Worker-3 quiere entrar
[t=0.0s] Worker-4 quiere entrar
[t=0.5s] Worker-0 escribe 1 y SALE
[t=0.5s] Worker-1 ENTRA y lee 1
[t=1.0s] Worker-1 escribe 2 y SALE
[t=1.0s] Worker-2 ENTRA y lee 2
[t=1.5s] Worker-2 escribe 3 y SALE
[t=1.5s] Worker-3 ENTRA y lee 3
[t=2.0s] Worker-3 escribe 4 y SALE
[t=2.0s] Worker-4 ENTRA y lee 4
[t=2.5s] Worker-4 escribe 5 y SALE

Peticiones atendidas: 5
Valor del contador:   5
```

### Explicación

#### Cómo funciona un `Lock`

Un `Lock` tiene dos estados: **libre** u **ocupado**.

| Operación | Si el lock está libre | Si el lock está ocupado |
|---|---|---|
| `acquire()` | Lo ocupa y el hilo continúa | El hilo **se bloquea** hasta que se libere |
| `release()` | — (error) | Lo libera; uno de los hilos que esperaban puede entrar |

#### Qué muestra la salida

- En `t=0.0s` **los 5 workers quieren entrar**, pero solo entra el Worker-0.
- Los demás **esperan bloqueados** en `acquire()`. No consumen CPU: simplemente no avanzan.
- Cada worker entra **justo cuando el anterior sale** (t = 0.5, 1.0, 1.5…).
- Cada worker lee el valor **ya actualizado** por el anterior.
- Resultado: **contador = 5**.

**Justificación:** el lock convierte la sección crítica en una zona de **un solo hilo cada vez**. Como ningún hilo puede leer mientras otro está entre su lectura y su escritura, el intercalado peligroso del programa 1A **se vuelve imposible**.

#### El precio de la exclusión

Antes, los 5 workers trabajaban a la vez (unos 0,5 s en total). Ahora, la sección crítica se ejecuta **en serie** (unos 2,5 s). Esto es inevitable para el código protegido, y es la razón por la que la sección crítica debe ser **lo más pequeña posible** (programa 2C).

#### El lock no impone un orden

En esta ejecución los workers entran en orden (0, 1, 2…), pero **el `Lock` no garantiza ningún orden**. Solo garantiza que entra **uno cada vez**. Otra ejecución, u otro sistema, puede dejar pasar primero al Worker-3. Si un programa necesita un orden concreto, hace falta otro mecanismo.

```text
Lock garantiza:     UNO CADA VEZ
Lock NO garantiza:  EN ORDEN
```

---

## Programa 1C — Por qué usar `with`

### Objetivo

Mostrar que `acquire()`/`release()` tiene un **riesgo grave**: si se produce una excepción dentro de la sección crítica, `release()` no se ejecuta y **el lock queda ocupado para siempre**.

### Código

```python
# Programa 1C — Por qué usar "with" en lugar de acquire()/release()
# -----------------------------------------------------------------
# Se ejecutan dos escenarios idénticos en los que el Worker-1 falla
# (lanza una excepción) DENTRO de la sección crítica:
#
#   Escenario 1: acquire() / release()
#   Escenario 2: with mutex:
#
# Se observa qué les ocurre a los workers que vienen detrás.

import threading
import time

contador_peticiones = 0


# Por defecto, una excepción en un hilo imprime un traceback largo.
# Lo sustituimos por un mensaje de una línea para que la salida sea legible.
def informar_excepcion(args):
    print(f"   !! {args.thread.name} ha fallado: {args.exc_value}")


threading.excepthook = informar_excepcion


def procesar(nombre):
    """Trabajo de la sección crítica. El Worker-1 recibe una petición errónea."""
    global contador_peticiones

    time.sleep(0.3)

    if nombre == "Worker-1":
        raise ValueError("petición mal formada")

    contador_peticiones += 1


# ---------- Escenario 1: acquire() / release() ----------

def worker_acquire_release(mutex, retraso):
    nombre = threading.current_thread().name
    time.sleep(retraso)                         # escalonamos las llegadas

    # El timeout sólo sirve para que la DEMOSTRACIÓN no se quede colgada:
    # sin él, los workers esperarían para siempre.
    if not mutex.acquire(timeout=2):
        print(f"   {nombre}: lleva 2 s esperando... el lock NUNCA se liberó")
        return

    print(f"   {nombre} entra")
    procesar(nombre)        # si aquí se lanza una excepción...
    mutex.release()         # ...esta línea NO llega a ejecutarse
    print(f"   {nombre} sale")


# ---------- Escenario 2: with ----------

def worker_with(mutex, retraso):
    nombre = threading.current_thread().name
    time.sleep(retraso)

    with mutex:             # adquiere al entrar en el bloque...
        print(f"   {nombre} entra")
        procesar(nombre)
    # ...y libera al salir, TAMBIÉN si se ha producido una excepción
    print(f"   {nombre} sale")


def ejecutar_escenario(titulo, funcion_worker):
    global contador_peticiones
    contador_peticiones = 0
    mutex = threading.Lock()

    print(f"\n=== {titulo} ===")

    hilos = []
    for i in range(4):
        hilo = threading.Thread(
            target=funcion_worker,
            args=(mutex, i * 0.1),
            name=f"Worker-{i}"
        )
        hilos.append(hilo)
        hilo.start()

    for hilo in hilos:
        hilo.join()

    print(f"   Peticiones registradas: {contador_peticiones} "
          f"(se esperaban 3: todas menos la errónea)")


ejecutar_escenario("Escenario 1: acquire() / release()", worker_acquire_release)
ejecutar_escenario("Escenario 2: with mutex", worker_with)
```

### Salida obtenida

```text

=== Escenario 1: acquire() / release() ===
   Worker-0 entra
   Worker-0 sale
   Worker-1 entra
   !! Worker-1 ha fallado: petición mal formada
   Worker-2: lleva 2 s esperando... el lock NUNCA se liberó
   Worker-3: lleva 2 s esperando... el lock NUNCA se liberó
   Peticiones registradas: 1 (se esperaban 3: todas menos la errónea)

=== Escenario 2: with mutex ===
   Worker-0 entra
   Worker-0 sale
   Worker-1 entra
   !! Worker-1 ha fallado: petición mal formada
   Worker-2 entra
   Worker-2 sale
   Worker-3 entra
   Worker-3 sale
   Peticiones registradas: 3 (se esperaban 3: todas menos la errónea)
```

### Explicación

#### Escenario 1: `acquire()` / `release()`

1. El Worker-0 entra, procesa y sale con normalidad.
2. El Worker-1 entra y **lanza una excepción** dentro de `procesar()`.
3. La excepción interrumpe la función: **la línea `mutex.release()` no se ejecuta**.
4. El lock queda **ocupado por un hilo que ya no existe**.
5. El Worker-2 y el Worker-3 esperan indefinidamente. Solo el `timeout` de la demostración les permite rendirse.

Resultado: **1 petición registrada de 3 posibles**. Un único error ha **bloqueado el servidor entero**.

> **Sobre el `timeout=2`:** sólo está para que la clase no se quede colgada. En un programa real con `acquire()` sin timeout, los workers **se quedarían esperando para siempre** y el programa no terminaría nunca.

#### Escenario 2: `with mutex`

```python
with mutex:
    ...
```

equivale a:

```text
ADQUIRIR  →  SECCIÓN CRÍTICA  →  LIBERAR (siempre, incluso si hay una excepción)
```

El Worker-1 falla igual que antes, pero **el lock se libera automáticamente** al salir del bloque `with`, también cuando se sale por una excepción. Los demás workers continúan con normalidad.

Resultado: **3 peticiones registradas**, las que debían. El error afecta solo a la petición errónea.

#### Conclusión

| | `acquire()` / `release()` | `with mutex:` |
|---|---|---|
| Delimita la sección crítica | Con dos líneas que pueden separarse o olvidarse | Con la indentación: se ve a simple vista |
| Si hay una excepción | El lock **queda ocupado** | El lock **se libera** |
| Si se olvida liberar | Bloqueo permanente | Imposible olvidarlo |

**Regla:** usar siempre `with` salvo que haya una razón concreta para no hacerlo, por ejemplo, necesitar un `timeout`.

---

# Ejemplo 2 — Usar bien el lock: tres errores frecuentes

Tener un `Lock` en el programa **no garantiza** que el programa sea correcto. Los tres programas de este ejemplo **usan un lock** y, sin embargo, tienen un problema.

```text
2A  El lock no cubre toda la operación   →  resultado INCORRECTO
2B  Dos locks para el mismo recurso      →  resultado INCORRECTO
2C  El lock cubre demasiado              →  resultado correcto, pero LENTO
```

---

## Programa 2A — El lock no cubre toda la operación

### Objetivo

Mostrar que el lock debe proteger la operación **completa** leer-modificar-escribir, y no solo la escritura.

### Código

```python
# Programa 2A — Error 1: el lock no cubre toda la operación
# ---------------------------------------------------------
# Hay un Lock, pero sólo protege la ESCRITURA.
# La LECTURA queda fuera.

import threading
import time

contador_peticiones = 0
mutex = threading.Lock()


def atender_peticion():
    global contador_peticiones
    nombre = threading.current_thread().name

    valor_leido = contador_peticiones          # LEER  -> ¡fuera del lock!
    print(f"{nombre} lee {valor_leido}")

    time.sleep(0.5)

    with mutex:                                # sólo MODIFICAR + ESCRIBIR están protegidos
        contador_peticiones = valor_leido + 1
        print(f"{nombre} escribe {contador_peticiones}")


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
Worker-0 lee 0
Worker-1 lee 0
Worker-2 lee 0
Worker-3 lee 0
Worker-4 lee 0
Worker-0 escribe 1
Worker-2 escribe 1
Worker-1 escribe 1
Worker-3 escribe 1
Worker-4 escribe 1

Peticiones atendidas: 5
Valor del contador:   1
```

### Explicación

El resultado es **exactamente igual de incorrecto** que sin lock: contador = 1.

**¿Por qué?** El lock impide que dos hilos **escriban** a la vez, pero no impide que **lean** a la vez. Los 5 workers leen 0 antes de que ninguno haya escrito. Después escriben de uno en uno, ordenadamente, **pero todos escriben 1**.

```text
Worker-0 lee 0  ─┐
Worker-1 lee 0   │  lecturas SIN protección
...              │
Worker-4 lee 0  ─┘
Worker-0 escribe 1  ─┐
Worker-1 escribe 1   │  escrituras protegidas... pero con un valor ya obsoleto
...                 ─┘
```

**Justificación:** lo que hay que hacer indivisible es la **operación lógica** "incrementar el contador", que incluye la lectura. Proteger solo una parte da una **falsa sensación de seguridad**: el código parece protegido, pero no lo está.

**Regla:** el lock debe abarcar **desde la primera lectura hasta la última escritura** del dato compartido que forman parte de la misma operación.

---

## Programa 2B — Dos locks distintos para el mismo recurso

### Objetivo

Mostrar que la exclusión mutua solo se produce entre hilos que usan **el mismo** lock.

### Código

```python
# Programa 2B — Error 2: dos locks distintos para el mismo recurso
# ----------------------------------------------------------------
# El servidor tiene dos tipos de peticiones (web y API).
# Cada función se escribió por separado y cada una creó "su" lock,
# pero las dos modifican el MISMO contador.

import threading
import time

contador_peticiones = 0

lock_web = threading.Lock()     # lo usa sólo atender_web()
lock_api = threading.Lock()     # lo usa sólo atender_api()


def incrementar(nombre):
    """Leer-modificar-escribir del contador compartido."""
    global contador_peticiones
    valor_leido = contador_peticiones
    time.sleep(0.5)
    contador_peticiones = valor_leido + 1
    print(f"{nombre} escribe {contador_peticiones}")


def atender_web():
    with lock_web:              # excluye a otras peticiones WEB...
        incrementar(threading.current_thread().name)


def atender_api():
    with lock_api:              # ...y éste a otras peticiones API, pero no a las web
        incrementar(threading.current_thread().name)


hilos = []

for i in range(3):
    hilos.append(threading.Thread(target=atender_web, name=f"Web-{i}"))
    hilos.append(threading.Thread(target=atender_api, name=f"API-{i}"))

for hilo in hilos:
    hilo.start()

for hilo in hilos:
    hilo.join()

print("\nPeticiones atendidas: 6")
print("Valor del contador:  ", contador_peticiones)
```

### Salida obtenida

```text
API-0 escribe 1
Web-0 escribe 1
Web-1 escribe 2
API-1 escribe 2
API-2 escribe 3
Web-2 escribe 3

Peticiones atendidas: 6
Valor del contador:   3
```

### Explicación

Se esperaban 6 peticiones y el contador marca **3**.

- Las tres peticiones **web** se excluyen entre sí (comparten `lock_web`).
- Las tres peticiones **API** se excluyen entre sí (comparten `lock_api`).
- Pero una petición web y una API **pueden estar a la vez** en la sección crítica, porque cada una tiene "su" lock. Así, en cada momento hay un worker web y uno API pisándose.

Un lock **no protege un dato**: protege un **fragmento de código** frente a otros fragmentos que usen **ese mismo lock**. Python no sabe qué variable "pertenece" a cada lock. Esa relación la decide el programador.

**Por qué este error es peligroso:** el resultado no es totalmente incorrecto (3 de 6, no 1 de 6). En un sistema real podría pasar inadvertido durante mucho tiempo.

**Regla:**

```text
TODAS las operaciones que acceden al MISMO recurso compartido
            ↓
deben usar el MISMO lock
```

---

## Programa 2C — Un lock demasiado grande

### Objetivo

Mostrar que un lock **correcto** también puede usarse **mal**: si protege más código del necesario, elimina la concurrencia.

### Código

```python
# Programa 2C — Error 3: un lock demasiado grande
# ----------------------------------------------
# Las dos versiones son CORRECTAS (el contador acaba bien).
# La diferencia está en CUÁNTO tiempo se tiene el lock.

import threading
import time

NUM_WORKERS = 4
DURACION_TRABAJO = 1.0          # segundos de trabajo independiente por petición

contador_peticiones = 0
mutex = threading.Lock()


def worker_lock_grande():
    global contador_peticiones
    with mutex:
        time.sleep(DURACION_TRABAJO)    # trabajo independiente... ¡dentro del lock!
        contador_peticiones += 1        # lo único que realmente necesita protección


def worker_lock_ajustado():
    global contador_peticiones
    time.sleep(DURACION_TRABAJO)        # trabajo independiente: en paralelo
    with mutex:
        contador_peticiones += 1        # sección crítica mínima


def medir(titulo, funcion_worker):
    global contador_peticiones
    contador_peticiones = 0

    inicio = time.perf_counter()

    hilos = [threading.Thread(target=funcion_worker) for _ in range(NUM_WORKERS)]
    for hilo in hilos:
        hilo.start()
    for hilo in hilos:
        hilo.join()

    duracion = time.perf_counter() - inicio
    print(f"{titulo:<15} contador = {contador_peticiones}   tiempo = {duracion:.1f} s")


medir("Lock grande:", worker_lock_grande)
medir("Lock ajustado:", worker_lock_ajustado)
```

### Salida obtenida

```text
Lock grande:    contador = 4   tiempo = 4.0 s
Lock ajustado:  contador = 4   tiempo = 1.0 s
```

### Explicación

Las dos versiones dan **el resultado correcto** (contador = 4). La diferencia está en el tiempo:

| Versión | Qué protege el lock | Tiempo |
|---|---|---:|
| Lock grande | Trabajo independiente + actualización del contador | **4,0 s** |
| Lock ajustado | Solo la actualización del contador | **1,0 s** |

```text
LOCK GRANDE                         LOCK AJUSTADO

W0 [trabajo + contador]             W0 [trabajo] [c]
W1          [trabajo + contador]    W1 [trabajo]  [c]
W2                   [trabajo + …]  W2 [trabajo]   [c]
W3                            [… ]  W3 [trabajo]    [c]

   → todo en serie: 4 × 1 s            → trabajo en paralelo: ≈ 1 s
```

**Justificación:** el trabajo independiente (procesar la petición) no toca ningún dato compartido, así que **no necesita protección**. Meterlo dentro del lock obliga a los workers a hacerlo de uno en uno y convierte un programa concurrente en uno secuencial. Con 100 workers, la diferencia sería de 100 s frente a 1 s.

**Regla:**

```text
PROTEGER TODO LO NECESARIO  (programa 2A)
PERO SÓLO LO NECESARIO      (programa 2C)
```

---

# Ejemplo 3 — Caso integrador: estadísticas del servidor

### Objetivo

Aplicar todo lo anterior a un programa más realista. Ya no se trata de un contador aislado, sino de un **estado compartido** formado por varios datos:

- `estadisticas["procesadas"]`
- `estadisticas["errores"]`
- la lista `resultados`

---

## Programa 3A — Ejercicio: versión sin proteger

### Enunciado para el alumno

1. Ejecuta el programa varias veces y anota los resultados.
2. Identifica los **recursos compartidos**.
3. Identifica qué parte es **trabajo independiente** y cuál es la **sección crítica**.
4. Añade un `Lock` para que el programa sea correcto. Ten en cuenta los tres errores del Ejemplo 2.
5. Justifica dónde lo has colocado y por qué.

### Código

```python
# Programa 3A — Servidor con estadísticas (SIN PROTEGER)
# ------------------------------------------------------
# EJERCICIO: este programa tiene condiciones de carrera.
# Identifica la sección crítica y protégela con un Lock.

import threading
import time
import random

NUM_PETICIONES = 10

# Recursos compartidos por todos los workers
estadisticas = {"procesadas": 0, "errores": 0}
resultados = []


def es_erronea(id_peticion):
    """Regla fija para saber de antemano el resultado correcto:
    las peticiones 3 y 7 fallan."""
    return id_peticion % 4 == 3


def procesar_peticion(id_peticion):
    nombre = threading.current_thread().name

    # --- Trabajo independiente: cada worker con sus propios datos ---
    time.sleep(random.uniform(0.5, 1.0))
    hubo_error = es_erronea(id_peticion)
    respuesta = "ERROR" if hubo_error else "OK"

    # --- Actualización de las estadísticas compartidas ---
    procesadas = estadisticas["procesadas"]
    errores = estadisticas["errores"]

    time.sleep(0.1)     # simula el coste de registrar (p. ej., escribir en un log)

    estadisticas["procesadas"] = procesadas + 1
    if hubo_error:
        estadisticas["errores"] = errores + 1

    resultados.append(f"{id_peticion}:{respuesta}")

    print(f"{nombre} termina petición {id_peticion} ({respuesta})")


hilos = []

for i in range(NUM_PETICIONES):
    hilo = threading.Thread(target=procesar_peticion, args=(i,), name=f"Worker-{i}")
    hilos.append(hilo)
    hilo.start()

for hilo in hilos:
    hilo.join()

print("\n--- ESTADÍSTICAS ---")
print(f"Procesadas: {estadisticas['procesadas']}  (esperado: 10)")
print(f"Errores:    {estadisticas['errores']}  (esperado: 2)")
print(f"Resultados: {resultados}")
```

### Salida obtenida

```text
Worker-0 termina petición 0 (OK)
Worker-5 termina petición 5 (OK)
Worker-4 termina petición 4 (OK)
Worker-8 termina petición 8 (OK)
Worker-7 termina petición 7 (ERROR)
Worker-6 termina petición 6 (OK)
Worker-9 termina petición 9 (OK)
Worker-3 termina petición 3 (ERROR)
Worker-2 termina petición 2 (OK)
Worker-1 termina petición 1 (OK)

--- ESTADÍSTICAS ---
Procesadas: 4  (esperado: 10)
Errores:    1  (esperado: 2)
Resultados: ['0:OK', '5:OK', '4:OK', '8:OK', '7:ERROR', '6:OK', '9:OK', '3:ERROR', '2:OK', '1:OK']
```

### Qué debe observar el alumno

- **`procesadas`** sale incorrecto en **todas** las ejecuciones (3 o 4 en lugar de 10).
- **`errores`** a veces sale bien (2) y a veces mal (1). Que un dato salga bien **no demuestra que el código sea correcto**: solo que en esa ejecución no hubo un intercalado peligroso.
- **`resultados`** tiene los 10 elementos, pero **en un orden distinto** al de las peticiones. El orden de terminación depende de cuánto tarda cada una.
- El estado final es **incoherente**: hay 10 resultados pero solo 4 peticiones "procesadas".

---

## Programa 3B — Solución

### Código

```python
# Programa 3B — Servidor con estadísticas (SOLUCIÓN)
# --------------------------------------------------
# Mismo programa que 3A con un único Lock que protege
# toda la actualización del estado compartido.

import threading
import time
import random

NUM_PETICIONES = 10

estadisticas = {"procesadas": 0, "errores": 0}
resultados = []

# UN SOLO lock para TODO el estado compartido: estadísticas y resultados
# deben actualizarse juntos y de forma coherente.
mutex = threading.Lock()


def es_erronea(id_peticion):
    """Las peticiones 3 y 7 fallan."""
    return id_peticion % 4 == 3


def procesar_peticion(id_peticion):
    nombre = threading.current_thread().name

    # --- Trabajo independiente: FUERA del lock (se ejecuta en paralelo) ---
    time.sleep(random.uniform(0.5, 1.0))
    hubo_error = es_erronea(id_peticion)
    respuesta = "ERROR" if hubo_error else "OK"

    # --- Sección crítica: TODA la operación leer-modificar-escribir ---
    with mutex:
        procesadas = estadisticas["procesadas"]
        errores = estadisticas["errores"]

        time.sleep(0.1)

        estadisticas["procesadas"] = procesadas + 1
        if hubo_error:
            estadisticas["errores"] = errores + 1

        resultados.append(f"{id_peticion}:{respuesta}")

    # Fuera del lock: el print no modifica estado compartido
    print(f"{nombre} termina petición {id_peticion} ({respuesta})")


inicio = time.perf_counter()

hilos = []

for i in range(NUM_PETICIONES):
    hilo = threading.Thread(target=procesar_peticion, args=(i,), name=f"Worker-{i}")
    hilos.append(hilo)
    hilo.start()

for hilo in hilos:
    hilo.join()

print("\n--- ESTADÍSTICAS ---")
print(f"Procesadas: {estadisticas['procesadas']}  (esperado: 10)")
print(f"Errores:    {estadisticas['errores']}  (esperado: 2)")
print(f"Resultados: {resultados}")
print(f"Tiempo total: {time.perf_counter() - inicio:.1f} s")
```

### Salida obtenida

```text
Worker-4 termina petición 4 (OK)
Worker-1 termina petición 1 (OK)
Worker-0 termina petición 0 (OK)
Worker-8 termina petición 8 (OK)
Worker-7 termina petición 7 (ERROR)
Worker-6 termina petición 6 (OK)
Worker-2 termina petición 2 (OK)
Worker-9 termina petición 9 (OK)
Worker-5 termina petición 5 (OK)
Worker-3 termina petición 3 (ERROR)

--- ESTADÍSTICAS ---
Procesadas: 10  (esperado: 10)
Errores:    2  (esperado: 2)
Resultados: ['4:OK', '1:OK', '0:OK', '8:OK', '7:ERROR', '6:OK', '2:OK', '9:OK', '5:OK', '3:ERROR']
Tiempo total: 1.6 s
```

> Se ha comprobado en ejecuciones repetidas: `procesadas` = 10 y `errores` = 2 **siempre**. El orden de `resultados` cambia en cada ejecución.

### Justificación de cada decisión

| Decisión | Justificación | Error que evita |
|---|---|---|
| **Un único lock** (`mutex`) para estadísticas y resultados | Los tres datos describen el mismo estado del servidor y deben actualizarse juntos | Programa 2B (locks distintos) |
| La **lectura** de `procesadas` y `errores` está **dentro** del lock | Leer-modificar-escribir es una operación lógica indivisible | Programa 2A (lock incompleto) |
| El **trabajo independiente** (`sleep`, calcular `respuesta`) está **fuera** del lock | No toca datos compartidos: puede ejecutarse en paralelo | Programa 2C (lock demasiado grande) |
| `resultados.append` está **dentro** del lock | Para que estadísticas y resultados sean siempre **coherentes entre sí** | Estado incoherente del 3A |
| El `print` final está **fuera** del lock | No modifica estado compartido; meterlo alargaría la sección crítica sin motivo | Programa 2C |
| Se usa **`with`** y no `acquire()`/`release()` | Si algo falla dentro, el lock se libera igualmente | Programa 1C |

### Rendimiento

El programa tarda unos **1,6 s**: hasta 1 s de trabajo en paralelo más 10 × 0,1 s de sección crítica en serie, que se solapan en parte con el trabajo de otros workers.

Si el trabajo independiente se hubiera metido dentro del lock, el tiempo subiría a unos **8 s** (10 × ~0,85 s).

### El orden de `resultados`

El orden varía entre ejecuciones **y es correcto que varíe**. El lock garantiza que solo un worker actualiza el estado a la vez, **no** en qué orden lo hacen. Si se necesitaran los resultados ordenados, habría que ordenarlos al final (`sorted(...)`), no esperar que el lock los ordene.

---

# Resumen de la sesión

```text
1A  Sección crítica = desde LEER hasta ESCRIBIR el dato compartido
    join() espera a que acaben los hilos; NO evita que se pisen

1B  Lock: un solo hilo cada vez en la sección crítica
    El lock NO garantiza ningún orden

1C  with libera el lock SIEMPRE, también ante excepciones

2A  Proteger TODA la operación lógica
2B  El MISMO recurso → el MISMO lock
2C  Proteger SÓLO lo necesario

3   Todo junto en un caso realista
```

# Continuidad con las siguientes sesiones

| Lo que queda abierto | Se verá en |
|---|---|
| Programa 1C de la Sesión 1 (lectores que ven estados intermedios). ¿Deben los lectores excluirse entre sí, o solo de los escritores? | Sesión 4 |
| Productor-consumidor (Sesión 1, Ejemplo 3). Un `Lock` evita que se pisen, pero **no** permite esperar a que haya datos | Sesión 5 |
| Pool de conexiones (Sesión 1, Ejemplo 2). Un `Lock` deja pasar a **1**; ¿y si queremos dejar pasar a **3**? | Sesión 6 |
