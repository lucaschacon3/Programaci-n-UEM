"""
EJEMPLO 1 - La barrera como punto de encuentro: la salida de una carrera
=========================================================================

Escenario: 5 corredores (hilos) se preparan para una carrera. Cada uno
tarda un tiempo distinto en calentar y colocarse en la línea de salida.

  - SIN barrera: cada corredor sale en cuanto termina de calentar.
    El primero puede llevar casi 2 segundos de ventaja: carrera injusta.
  - CON barrera: cada corredor llama a barrera.wait() en la línea de
    salida y se queda bloqueado hasta que llegan los 5. Entonces TODOS
    se desbloquean a la vez y salen juntos.

Idea clave:
  threading.Barrier(n) crea un punto de encuentro para n hilos.
  wait() bloquea al hilo hasta que n hilos hayan llamado a wait().
  Ningún hilo pasa de ese punto hasta que han llegado todos.

Diferencia con el semáforo: el semáforo limita CUÁNTOS hilos pueden
entrar; la barrera decide CUÁNDO pueden seguir todos.
"""

import random
import threading
import time

N_CORREDORES = 5


def log(mensaje):
    print(f"[{time.perf_counter() - T0:5.2f}s] {mensaje}")


def corredor(nombre, barrera, salidas):
    calentamiento = random.uniform(0.2, 2.0)
    time.sleep(calentamiento)                 # Calentando...
    log(f"{nombre} llega a la línea de salida (calentó {calentamiento:.1f}s)")

    if barrera is not None:
        barrera.wait()                        # Espera a que lleguen TODOS

    momento = time.perf_counter() - T0
    salidas.append(momento)
    log(f"   {nombre} ¡SALE!")


def carrera(con_barrera):
    global T0
    titulo = "CON barrera" if con_barrera else "SIN barrera"
    print(f"\n========== CARRERA {titulo} ==========")

    barrera = threading.Barrier(N_CORREDORES) if con_barrera else None
    salidas = []
    T0 = time.perf_counter()

    hilos = [threading.Thread(target=corredor,
                              args=(f"Corredor-{i}", barrera, salidas))
             for i in range(1, N_CORREDORES + 1)]
    for h in hilos:
        h.start()
    for h in hilos:
        h.join()

    diferencia = max(salidas) - min(salidas)
    print(f"--> Diferencia entre el primero y el último en salir: {diferencia:.3f} s")


if __name__ == "__main__":
    carrera(con_barrera=False)
    carrera(con_barrera=True)


# ===========================================================================
# PARA EL AULA
# ---------------------------------------------------------------------------
# - Sin barrera, las salidas están repartidas a lo largo de casi 2 segundos.
#   Con barrera, todos salen en el mismo instante (diferencia de milésimas).
# - Mientras esperan en wait(), los hilos están BLOQUEADOS: no consumen CPU.
#   No es un bucle "while not todos_listos: pass" (espera activa).
# - El número que se pasa a Barrier(n) debe coincidir con el número de hilos
#   que van a llamar a wait(). ¿Qué pasa si no coincide? (pregunta 1)
#
# PREGUNTAS / VARIACIONES
# 1. Cread la barrera con Barrier(N_CORREDORES + 1). ¿Qué le ocurre al
#    programa? ¿Por qué? (Pista: alguien está esperando a un corredor que
#    no existe. Pulsad Ctrl+C para pararlo.)
# 2. Y con Barrier(N_CORREDORES - 1), ¿salen todos a la vez?
# 3. Ejercicio 1 del tema: completad la carrera para que, tras la salida
#    justa, los corredores avancen y SOLO UNO pueda proclamarse ganador.
#    ¿Qué mecanismo usaríais para eso? ¿Sirve una barrera?
# ===========================================================================
