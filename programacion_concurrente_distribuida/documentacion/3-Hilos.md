# Sesión 3 — Hilos y modelos de ejecución

**Programación Concurrente · Alumnos / Proyección**

> ¿Cómo organiza una aplicación varias actividades dentro del mismo proceso?

---

## Qué vamos a comprender

- Qué es un hilo y a qué proceso pertenece.
- Modelos monohilo y multihilo.
- Memoria compartida: ventajas y riesgos.
- Relación entre procesos, hilos y Python.

---

## Punto de partida: un editor de código

```text
Editor
├── responder al teclado
├── colorear sintaxis
├── guardar automáticamente
├── comprobar errores
└── acceder a Git
```

> ¿Cómo separar estas actividades para que una espera no detenga toda la aplicación?

---

## ¿Qué es un hilo?

> **Un hilo es una unidad de ejecución dentro de un proceso.**

```text
Proceso
├── Hilo 1 → secuencia de instrucciones
├── Hilo 2 → secuencia de instrucciones
└── Hilo 3 → secuencia de instrucciones
```

El proceso proporciona el entorno, la memoria y los recursos.

---

## Modelo monohilo

```text
Proceso A
└── Hilo 1
    Leer → Procesar → Guardar → Actualizar interfaz
```

Una sola secuencia de ejecución.

Si realiza una operación bloqueante, esa secuencia debe esperar.

---

## Modelo multihilo

Diseño conceptual de un editor:

```text
Proceso A
├── Hilo 1 → interfaz
├── Hilo 2 → autoguardado
├── Hilo 3 → análisis de sintaxis
└── Hilo 4 → Git
```

Cada hilo puede encargarse de una actividad distinta.

---

## ¿Qué comparten los hilos?

```text
Hilo 1 ──┐
Hilo 2 ──┼──► Memoria y recursos del proceso
Hilo 3 ──┘
```

Comparten el espacio de memoria, pero cada hilo mantiene su propio estado de ejecución.

**Compartir memoria no significa ejecutar la misma instrucción.**

---

## Procesos frente a hilos

| Aspecto | Procesos distintos | Hilos del mismo proceso |
|---|---|---|
| Espacio de memoria | Separado por defecto | Compartido |
| Aislamiento | Mayor | Menor |
| Compartir datos | Requiere mecanismos explícitos | Pueden acceder a los mismos datos |
| Organización | Ejecuciones independientes | Secuencias dentro del proceso |

---

## Navegador y reproductor multimedia

Posibles diseños simplificados:

```text
Navegador                 Reproductor
├── interfaz              ├── interfaz
├── descarga              ├── lectura de archivo
├── procesamiento         ├── decodificación
└── eventos               └── salida de audio
```

Mientras una actividad espera datos, otras pueden avanzar si no dependen de ellos.

---

## Servidor web

```text
Proceso servidor
├── Hilo A → petición del cliente A
├── Hilo B → petición del cliente B
└── Hilo C → petición del cliente C
```

```text
A espera una consulta a BD
B puede procesar su petición
C puede preparar su respuesta
```

Una petición lenta no tiene por qué detener las demás.

---

## Secuencias y dependencias

Código conceptual para un archivo:

```python
descargar_archivo()
procesar_datos()
guardar_resultado()
```

```text
Archivo A: descargar → procesar → guardar
Archivo B: descargar → procesar → guardar
Archivo C: descargar → procesar → guardar
```

Distintos archivos pueden gestionarse con distintos hilos.

**Las dependencias de cada archivo siguen existiendo.**

---

## Cada hilo conserva su secuencia

```text
Hilo A: A1 → A2 → A3
Hilo B: B1 → B2 → B3

Un intercalado posible:
A1 → B1 → A2 → B2 → B3 → A3
```

El orden global puede variar aunque cada hilo siga su secuencia.

---

## Cuatro modelos de ejecución

```text
1 proceso / 1 hilo       1 proceso / varios hilos
P1                      P1
└── H1                  ├── H1
                        └── H2

Varios procesos         Varios procesos
1 hilo por proceso      varios hilos por proceso
P1 ── H1                P1 ── H1, H2
P2 ── H2                P2 ── H3, H4
```

Procesos e hilos describen niveles distintos de organización.

---

## Memoria compartida: colaborar

```text
Proceso servidor

Hilo A ──┐
Hilo B ──┼──► cache = {}
Hilo C ──┘
```

Los hilos pueden reutilizar una misma caché sin mantener copias independientes.

---

## Memoria compartida: coordinar

```python
peticiones_procesadas = 100
```

Dos hilos quieren ejecutar:

```python
peticiones_procesadas += 1
```

> ¿Qué riesgo aparece al modificar el mismo dato sin coordinación?

```text
Datos compartidos → posibles interferencias → sincronización
```

---

## Ventajas y dificultades

| Ventajas | Dificultades |
|---|---|
| Separar actividades | Razonar sobre intercalados |
| Mantener capacidad de respuesta | Coordinar datos compartidos |
| Aprovechar esperas | Evitar condiciones de carrera |
| Cooperar mediante memoria común | Mayor complejidad |

**Más hilos no garantizan un programa mejor ni más rápido.**

---

## Actividad: identifica el modelo

1. Dos ejecuciones independientes de un script Python.
2. Un proceso con hilos de interfaz, red y logs.
3. Tres procesos, cada uno con dos hilos.
4. Un script con una sola secuencia de ejecución.

Dibuja cada estructura y señala dónde se comparte memoria.

---

## Actividad: aplicación de mensajería

Debe realizar:

```text
Mostrar interfaz · Recibir mensajes
Enviar mensajes · Reproducir notificaciones
```

- Propón una distribución de actividades entre hilos.
- Identifica datos que podrían compartir.
- Explica qué ocurre si la recepción espera datos de red.

---

## Actividad: editor y contador

**Caso A:** un editor monohilo tarda cinco segundos en una escritura bloqueante.

> ¿Cómo separarías actividades para mantener la interfaz disponible?

**Caso B:** dos hilos de un servidor incrementan un contador común.

> ¿Dónde está el contador? ¿Qué habría que coordinar?

---

## Puente hacia Python

```text
Python
├── multiprocessing → procesos
└── threading       → hilos
```

`threading` permite crear, iniciar y sincronizar hilos.

```text
Hilo A → procesar_peticion("Cliente A")
Hilo B → procesar_peticion("Cliente B")
Hilo C → procesar_peticion("Cliente C")
```

---

## Actividad final: sistema de monitorización

La aplicación debe:

1. Recoger métricas del sistema.
2. Escuchar eventos de red.
3. Escribir logs.
4. Actualizar la interfaz.

Dibuja un diseño multihilo y explica:

- Qué comparten los hilos.
- Qué actividades pueden esperar.
- Qué riesgo surge si todos modifican una estructura de datos común.

---

## Resumen

```text
Programa → Proceso → Hilos
              │        │
              └────────┴──► Memoria y recursos compartidos
```

- Un hilo ejecuta dentro de un proceso.
- Un proceso puede ser monohilo o multihilo.
- Compartir memoria facilita cooperar y exige coordinación.
- Varios hilos no implican varios núcleos trabajando a la vez.

**Siguiente sesión:** concurrencia frente a paralelismo.
