"""
EJEMPLO 1 - Semáforo binario como mutex: el contador compartido
================================================================

Punto de partida: el ejemplo de la diapositiva "Mecanismo para prevenirlo:
Semáforos" del Tema 2, donde varios hilos incrementan una variable compartida.

En este ejemplo hacemos dos ejecuciones:
  1) SIN semáforo: forzamos la condición de carrera para que se vea el error.
  2) CON semáforo (valor inicial 1): el resultado siempre es correcto.

Idea clave: un semáforo es un contador interno de "permisos".
  - acquire(): si el contador es > 0 lo decrementa y sigue; si es 0, el hilo
    se BLOQUEA hasta que otro hilo haga release().
  - release(): incrementa el contador y despierta a un hilo que esté esperando.
Con valor inicial 1 solo un hilo puede estar dentro a la vez: se comporta
igual que un mutex (threading.Lock). Por eso se le llama "semáforo binario".
"""

import threading
import time

N_HILOS = 10
INCREMENTOS_POR_HILO = 100
ESPERADO = N_HILOS * INCREMENTOS_POR_HILO

contador = 0

# Semáforo binario. threading.Semaphore() sin argumentos también vale 1,
# pero lo escribimos explícito para que se vea el valor inicial.
semaforo = threading.Semaphore(1)


# ---------------------------------------------------------------------------
# Versión 1: SIN protección
# ---------------------------------------------------------------------------
def incrementar_sin_proteccion():
    global contador
    for _ in range(INCREMENTOS_POR_HILO):
        valor = contador          # 1. LEER el valor compartido
        time.sleep(0.00001)       # 2. Pausa: forzamos que otro hilo se cuele aquí
        contador = valor + 1      # 3. ESCRIBIR el valor (¡puede estar desfasado!)


# ---------------------------------------------------------------------------
# Versión 2: CON semáforo (forma recomendada: sentencia with)
# ---------------------------------------------------------------------------
def incrementar_con_semaforo():
    global contador
    for _ in range(INCREMENTOS_POR_HILO):
        with semaforo:            # acquire() al entrar, release() al salir
            valor = contador
            time.sleep(0.00001)   # La misma pausa, pero ahora nadie puede colarse
            contador = valor + 1


# ---------------------------------------------------------------------------
# Versión 2b: lo mismo escrito con acquire()/release() explícitos.
# El try/finally garantiza que el permiso se devuelve aunque haya una excepción.
# ---------------------------------------------------------------------------
def incrementar_con_acquire_release():
    global contador
    for _ in range(INCREMENTOS_POR_HILO):
        semaforo.acquire()
        try:
            valor = contador
            time.sleep(0.00001)
            contador = valor + 1
        finally:
            semaforo.release()


def ejecutar(funcion, titulo):
    global contador
    contador = 0
    hilos = [threading.Thread(target=funcion) for _ in range(N_HILOS)]

    inicio = time.perf_counter()
    for h in hilos:
        h.start()
    for h in hilos:
        h.join()
    duracion = time.perf_counter() - inicio

    estado = "CORRECTO" if contador == ESPERADO else "INCORRECTO (condición de carrera)"
    print(f"{titulo:<38} -> contador = {contador:>4} / {ESPERADO}  "
          f"[{estado}]  ({duracion:.2f} s)")


if __name__ == "__main__":
    print(f"{N_HILOS} hilos x {INCREMENTOS_POR_HILO} incrementos cada uno\n")
    ejecutar(incrementar_sin_proteccion, "Sin semáforo")
    ejecutar(incrementar_con_semaforo, "Con semáforo (with)")
    ejecutar(incrementar_con_acquire_release, "Con semáforo (acquire/release)")


# ===========================================================================
# PARA EL AULA
# ---------------------------------------------------------------------------
# - ¿Por qué falla la versión sin semáforo? Dos hilos leen el mismo valor
#   (p. ej. 41), ambos escriben 42 y se pierde un incremento.
# - La pausa time.sleep() solo sirve para que el error aparezca SIEMPRE. Sin
#   ella el fallo también puede ocurrir, pero de forma esporádica, que es
#   justo lo que hace tan peligrosas las condiciones de carrera.
# - Fijaos en el tiempo: la versión protegida es más lenta porque la sección
#   crítica se ejecuta en serie. La sincronización tiene un coste.
#
# PREGUNTAS / VARIACIONES
# 1. Quitad el time.sleep() de la versión sin semáforo y ejecutadla varias
#    veces. ¿Sale siempre bien? ¿Significa eso que el código es correcto?
# 2. Cambiad Semaphore(1) por Semaphore(2). ¿Qué ocurre con el resultado?
#    ¿Por qué un semáforo con valor > 1 ya no garantiza exclusión mutua?
# 3. Poned el "with semaforo:" fuera del bucle for. ¿Sigue siendo correcto?
#    ¿Qué cambia en la concurrencia real entre hilos?
# ===========================================================================
