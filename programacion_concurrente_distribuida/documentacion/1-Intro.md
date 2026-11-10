# Sesión 1 — De la ejecución secuencial a la necesidad de concurrencia

**Programación Concurrente · Alumnos / Proyección**

> ¿Por qué necesitamos concurrencia si ya podemos ejecutar instrucciones una detrás de otra?

---

## Qué vamos a comprender

- Cómo colaboran CPU, memoria y entrada/salida.
- Qué diferencia hay entre calcular y esperar.
- Cómo aprovechar las esperas para que otras tareas avancen.

---

## ¿Qué ocurre al ejecutar un programa?

```text
            CPU
    ┌────────┴────────┐
    ALU      Unidad de control
             ↕
           Buses
       ┌─────┴─────┐
       ↕           ↕
    Memoria       E/S
 instrucciones   disco
     datos       red, teclado, pantalla
```

- **CPU:** ejecuta instrucciones.
- **ALU:** realiza operaciones aritméticas y lógicas.
- **Unidad de control:** coordina su ejecución.
- **Buses:** transportan información y señales de control.

---

## Ejecución secuencial

```python
print("Inicio")
a = 10
b = 20
resultado = a + b
print(resultado)
print("Fin")
```

```text
Inicio → asignaciones → suma → resultado → Fin
```

**Una secuencia de instrucciones sigue el orden del programa.**

---

## Programa frente a ejecución

```text
servidor.py almacenado
          │
          │ python servidor.py
          ▼
Ejecución con memoria y recursos
```

**Programa:** instrucciones almacenadas.

**Ejecución:** esas instrucciones están siendo llevadas a cabo.

---

## Instrucciones, datos y memoria

```python
usuario = "Ana"
intentos = 3
autenticado = False
```

```text
CPU ←→ Memoria
         ├── instrucciones
         └── datos: "Ana", 3, False
```

La memoria conserva información necesaria durante la ejecución.

---

## Entrada/salida: no todo es calcular

**E/S** conecta el programa con dispositivos y servicios.

| Operación | Recurso |
|---|---|
| Leer un fichero | Disco |
| Descargar datos | Red |
| Consultar una BD remota | Red y servidor de BD |
| Recibir una tecla | Teclado |
| Imprimir un archivo | Impresora |

> ¿Qué puede hacer la CPU mientras una respuesta todavía no ha llegado?

---

## Consulta a una base de datos

Código conceptual:

```python
usuario = obtener_usuario_base_datos(1234)
resultado = calcular_descuento(usuario)
mostrar_resultado(resultado)
```

```text
Consultar BD        Calcular    Mostrar
   100 ms              2 ms       1 ms
[gran parte: espera] [trabajo]   [salida]
```

**Tiempo transcurrido y tiempo de CPU son magnitudes distintas.**

---

## CPU-bound frente a espera de E/S

| Predomina el cálculo | Predomina la espera |
|---|---|
| Ordenar una lista grande | Descargar un fichero |
| Comprimir datos | Esperar una API |
| Procesar una imagen | Esperar una BD remota |

```text
Cálculo:   CPU → CPU → CPU → CPU
E/S:       CPU → espera de respuesta → CPU
```

Una operación de E/S también necesita algo de CPU para iniciarse y finalizar.

---

## Servidor web: una petición

Modelo simplificado de una petición `GET /usuario/123`:

```text
Recibir → Esperar BD → Procesar → Enviar
 1 ms       80 ms        3 ms      2 ms
```

**Total aproximado: 86 ms.** En este modelo, unos 6 ms corresponden a trabajo local.

> Llega el cliente B mientras A espera. ¿Tiene que esperar a que A termine por completo?

---

## Aprovechar los periodos de espera

```text
Tiempo →
A:  CPU ─── espera de E/S ─── CPU
B:       CPU ─── espera de E/S ─── CPU
```

Mientras A espera, B puede utilizar la CPU si está preparado.

**Esperar una operación no obliga a detener todas las demás tareas.**

---

## Tres tareas informáticas

| Tarea | Duración aproximada |
|---|---:|
| Descargar un fichero | 4 s |
| Consultar una BD | 3 s |
| Consultar una API | 2 s |

```text
Secuencial:
Descarga [4 s] → BD [3 s] → API [2 s]
Total ≈ 9 s

Con esperas solapadas:
Descarga [──────── 4 s ────────]
BD       [───── 3 s ─────]
API      [── 2 s ──]
```

Las barras representan **tareas en progreso**, incluidas sus esperas.

---

## Primera idea de concurrencia

> **Varias tareas progresan durante el mismo intervalo temporal.**

No implica que todas estén usando la CPU en el mismo instante.

```text
Editor de código           Navegador
├── interfaz               ├── HTML y CSS
├── análisis de sintaxis    ├── imágenes
├── autoguardado            ├── vídeo
└── Git                     └── eventos de usuario
```

Son modelos de actividades: una espera no debería bloquear toda la interacción.

---

## El papel del sistema operativo

```text
Navegador   Editor   Terminal   Reproductor
     └────────┴────────┴────────┘
                      ↓
              Sistema operativo
                      ↓
              CPU · Memoria · E/S
```

El sistema operativo administra y reparte los recursos.

---

## Actividad: CPU o espera

Clasifica según qué predomina y justifica:

1. Multiplicar dos números.
2. Leer `datos.csv`.
3. Consultar una API remota.
4. Ordenar una lista muy grande.
5. Renderizar un vídeo.
6. Descargar diez imágenes.

---

## Actividad: dibuja una ejecución

| Tarea | CPU inicial | Espera | CPU final |
|---|---:|---:|---:|
| A | 2 ms | 8 ms | 2 ms |
| B | 3 ms | 5 ms | 2 ms |

Una sola unidad de procesamiento.

1. Dibuja A completa y después B.
2. Propón una ejecución que aproveche las esperas.
3. Señala cuándo la CPU queda libre.

---

## Actividad: tres peticiones

Cada petición requiere:

```text
5 ms CPU → 100 ms esperando BD → 5 ms CPU
```

- ¿Qué inconveniente tiene terminar A antes de iniciar B y C?
- ¿Qué trabajo puede avanzar durante la espera de A?
- ¿Qué operaciones de una misma petición dependen de las anteriores?

---

## Capacidad de respuesta y utilización

La concurrencia puede mejorar:

- **Capacidad de respuesta:** la interfaz sigue atendiendo eventos.
- **Aprovechamiento de recursos:** otra tarea usa la CPU durante una espera.
- **Tiempo total:** se pueden solapar operaciones independientes.

Una cola de impresión permite seguir trabajando mientras se imprimen documentos.

**La concurrencia no garantiza más velocidad:** gestionar tareas también tiene coste.

---

## Python: observar la ejecución secuencial

Simulación; no realiza descargas ni consultas reales.

```python
import time


def descargar_archivo():
    print("Iniciando descarga")
    time.sleep(3)
    print("Descarga terminada")


def consultar_base_datos():
    print("Consultando BD")
    time.sleep(2)
    print("Consulta terminada")


def consultar_api():
    print("Consultando API")
    time.sleep(1)
    print("API terminada")
```

---

## Ejecuta y observa

Añade al código anterior:

```python
inicio = time.time()

descargar_archivo()
consultar_base_datos()
consultar_api()

fin = time.time()
print(f"Tiempo total: {fin - inicio:.2f} segundos")
```

1. Predice el orden de los mensajes y el tiempo total.
2. Ejecuta y contrasta tu predicción.
3. ¿`sleep()` está realizando cálculos durante la pausa?

---

## Actividad final: copias de seguridad

Para cada uno de tres ficheros:

```text
Leer → Comprimir → Enviar al servidor → Registrar en BD
```

- Identifica cálculo y posibles esperas de E/S.
- Explica el inconveniente de completar cada fichero antes del siguiente.
- Propón operaciones que podrían solaparse respetando sus dependencias.

---

## Resumen

```text
CPU + Memoria + E/S
          ↓
Cálculo y periodos de espera
          ↓
Otras tareas pueden avanzar
          ↓
Concurrencia
```

**Comprueba:** ¿estar esperando significa que el programa ha terminado?

**Siguiente sesión:** el proceso y la gestión del sistema operativo.
