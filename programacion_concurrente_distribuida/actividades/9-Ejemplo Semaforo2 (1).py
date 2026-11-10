"""
EJEMPLO 2 - Semáforo contador: limitar el acceso a N hilos a la vez
====================================================================

Escenario: una sala de estudio de la biblioteca tiene 3 puestos.
Llegan 8 alumnos (hilos) que quieren estudiar un rato.

  - Un semáforo con valor inicial 3 representa los 3 puestos libres.
  - Cada alumno hace acquire() para ocupar un puesto y release() al salir.
  - Si los 3 puestos están ocupados, el alumno espera en la puerta.

Además, cada alumno tiene una "paciencia" limitada: usamos
acquire(timeout=...) para que, si no consigue sitio a tiempo, se vaya.
Es la misma idea del timeout que el tema usa con los mutex para evitar
esperas infinitas.

Usamos BoundedSemaphore en lugar de Semaphore: funciona igual, pero lanza
ValueError si se hace un release() de más (un error de programación que
"crearía" puestos que no existen).
"""

import random
import threading
import time

PUESTOS = 3
N_ALUMNOS = 8

puestos_libres = threading.BoundedSemaphore(PUESTOS)

# Contador auxiliar SOLO para mostrar por pantalla cuántos hay dentro.
# Como es una variable compartida, la protegemos con su propio Lock.
ocupados = 0
lock_ocupados = threading.Lock()


def log(mensaje):
    print(f"[{time.perf_counter() - T0:5.2f}s] {mensaje}")


def alumno(nombre):
    global ocupados
    paciencia = random.uniform(1.0, 4.0)       # Segundos que está dispuesto a esperar
    log(f"{nombre} llega a la sala (paciencia {paciencia:.1f}s)")

    # Intentamos coger un puesto, pero no esperamos indefinidamente
    if not puestos_libres.acquire(timeout=paciencia):
        log(f"   {nombre} se cansa de esperar y se va  :(")
        return

    try:
        with lock_ocupados:
            ocupados += 1
            log(f">> {nombre} se sienta.        Puestos ocupados: {ocupados}/{PUESTOS}")

        time.sleep(random.uniform(1.0, 2.5))   # Estudiando...

        with lock_ocupados:
            ocupados -= 1
            log(f"<< {nombre} deja su puesto.   Puestos ocupados: {ocupados}/{PUESTOS}")
    finally:
        puestos_libres.release()                # Devolvemos el puesto


if __name__ == "__main__":
    T0 = time.perf_counter()
    hilos = []
    for i in range(1, N_ALUMNOS + 1):
        h = threading.Thread(target=alumno, args=(f"Alumno-{i}",))
        hilos.append(h)
        h.start()
        time.sleep(0.2)                         # Los alumnos llegan escalonados

    for h in hilos:
        h.join()
    log("La sala cierra.")

    # Demostración de BoundedSemaphore: un release() de más es un error
    print("\nProbando un release() de más sobre el BoundedSemaphore...")
    try:
        puestos_libres.release()
    except ValueError as e:
        print("ValueError capturado:", e)


# ===========================================================================
# PARA EL AULA
# ---------------------------------------------------------------------------
# - Observad que "Puestos ocupados" nunca supera 3: el semáforo lo garantiza.
# - A diferencia del ejemplo 1, aquí SÍ queremos que varios hilos estén
#   dentro a la vez (hasta 3). Un mutex no sirve para esto.
# - acquire(timeout=t) devuelve True si consiguió el permiso y False si se
#   agotó el tiempo. Hay que comprobar siempre el valor devuelto.
# - El contador "ocupados" necesita su propio Lock: el semáforo limita
#   cuántos entran, pero NO protege las variables que esos hilos modifican.
#
# PREGUNTAS / VARIACIONES
# 1. Cambiad PUESTOS a 1. ¿En qué se convierte el ejemplo?
# 2. Quitad el timeout (acquire() sin argumentos). ¿Se va algún alumno?
# 3. Cambiad BoundedSemaphore por Semaphore y repetid la prueba final.
#    ¿Qué pasa? ¿Por qué es peligroso?
# 4. Ejercicio del tema: adaptad este código al restaurante con mesas.
# ===========================================================================
