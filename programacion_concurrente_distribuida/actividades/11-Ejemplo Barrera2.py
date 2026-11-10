"""
EJEMPLO 2b - Semáforo contador: un pool de conexiones a una base de datos
==========================================================================

Escenario: un servidor web solo puede tener 3 conexiones abiertas a la
vez con la base de datos (es lo que permite la licencia). Llegan 10
peticiones de usuarios (hilos) y cada una necesita una conexión para hacer
su consulta.

  - Un semáforo con valor inicial 3 representa las 3 conexiones libres.
  - Cada petición hace acquire() para obtener una conexión y release()
    al terminar su consulta.
  - Si las 3 conexiones están en uso, la petición espera su turno.

Además, ningún usuario quiere esperar eternamente a que cargue la página:
usamos acquire(timeout=...) y, si no hay conexión en 1 s, la petición
responde "503 - Servidor ocupado".

Novedad respecto al ejemplo 2: ahora no basta con saber CUÁNTAS conexiones
quedan libres, hay que saber CUÁLES. Guardamos las conexiones libres en
una lista protegida por un Lock:
  - el SEMÁFORO garantiza que, si entras, hay al menos una conexión libre;
  - el LOCK garantiza que dos peticiones no cogen la MISMA conexión.

Usamos BoundedSemaphore: si por error se devuelve una conexión de más,
lanza ValueError en lugar de "inventarse" una cuarta conexión.
"""

import random
import threading
import time

MAX_CONEXIONES = 3
N_PETICIONES = 10
TIEMPO_MAXIMO_ESPERA = 1.0          # Segundos antes de responder "503"

conexiones_disponibles = threading.BoundedSemaphore(MAX_CONEXIONES)

# Lista de conexiones libres. Es una estructura compartida: necesita su Lock.
conexiones_libres = [f"Conexión-{c}" for c in "ABC"]
lock_conexiones = threading.Lock()

# Estadísticas (también compartidas, protegidas por el mismo Lock)
atendidas = 0
rechazadas = 0


def log(mensaje):
    print(f"[{time.perf_counter() - T0:5.2f}s] {mensaje}")


def peticion(nombre):
    global atendidas, rechazadas
    log(f"{nombre} llega al servidor")

    # 1. Esperar a que haya alguna conexión libre (como mucho 1 s)
    if not conexiones_disponibles.acquire(timeout=TIEMPO_MAXIMO_ESPERA):
        with lock_conexiones:
            rechazadas += 1
        log(f"   {nombre}: sin conexión tras {TIEMPO_MAXIMO_ESPERA}s "
            f"-> 503 Servidor ocupado")
        return

    # 2. Sabemos que hay una libre: la sacamos de la lista
    with lock_conexiones:
        conexion = conexiones_libres.pop()
        en_uso = MAX_CONEXIONES - len(conexiones_libres)
        log(f">> {nombre} obtiene {conexion}      (en uso: {en_uso}/{MAX_CONEXIONES})")

    try:
        time.sleep(random.uniform(0.8, 1.8))      # Ejecutando la consulta SQL...
    finally:
        # 3. Devolver la conexión a la lista y el permiso al semáforo
        with lock_conexiones:
            conexiones_libres.append(conexion)
            atendidas += 1
            en_uso = MAX_CONEXIONES - len(conexiones_libres)
            log(f"<< {nombre} devuelve {conexion}     (en uso: {en_uso}/{MAX_CONEXIONES})")
        conexiones_disponibles.release()


if __name__ == "__main__":
    T0 = time.perf_counter()
    hilos = []
    for i in range(1, N_PETICIONES + 1):
        h = threading.Thread(target=peticion, args=(f"Petición-{i:02d}",))
        hilos.append(h)
        h.start()
        time.sleep(random.uniform(0.05, 0.25))    # Las peticiones llegan seguidas

    for h in hilos:
        h.join()

    print(f"\n--> Atendidas: {atendidas}   Rechazadas (503): {rechazadas}   "
          f"Conexiones libres al final: {sorted(conexiones_libres)}")

    # Demostración de BoundedSemaphore: devolver una conexión de más
    print("\nUn programador devuelve por error una conexión que no había cogido...")
    try:
        conexiones_disponibles.release()
    except ValueError as e:
        print("ValueError capturado:", e)


# ===========================================================================
# PARA EL AULA
# ---------------------------------------------------------------------------
# - "En uso" nunca supera 3: el semáforo lo garantiza.
# - Cada conexión la usa una sola petición a la vez: lo garantiza el Lock
#   sobre la lista. El semáforo NO lo garantizaría por sí solo: dos hilos
#   que pasan el acquire() a la vez podrían hacer pop() a la vez.
# - Fijaos en el ORDEN: primero acquire() del semáforo (¿hay alguna?) y
#   después el Lock para coger una concreta (¿cuál?). Al terminar, al revés:
#   primero se devuelve a la lista y después release().
# - Al final las 3 conexiones vuelven a estar libres: el finally asegura
#   que se devuelven aunque la consulta falle.
#
# PREGUNTAS / VARIACIONES
# 1. Cambiad MAX_CONEXIONES a 1 (y la lista a una sola conexión). ¿En qué
#    mecanismo se convierte el semáforo?
# 2. Subid TIEMPO_MAXIMO_ESPERA a 10. ¿Se rechaza alguna petición? ¿Qué
#    ventaja e inconveniente tiene para el usuario?
# 3. Quitad el "with lock_conexiones" alrededor del pop(). ¿Podrían dos
#    peticiones obtener la misma conexión? (Pista: pensad en una lista de
#    Python más compleja que pop(), como buscar la conexión "más rápida".)
# 4. ¿Qué pasaría si se hiciera release() ANTES de devolver la conexión a
#    la lista? Pensad en un hilo que entra justo en ese instante.
# ===========================================================================
