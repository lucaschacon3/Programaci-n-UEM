# Sesión 4 — Concurrencia frente a paralelismo

**Programación Concurrente · Alumnos / Proyección**

> Si varias tareas avanzan durante un intervalo, ¿se ejecutan necesariamente en el mismo instante?

---

## Qué vamos a distinguir

- Ejecución secuencial, concurrente y paralela.
- Intercalado y simultaneidad real.
- Trabajo dominado por espera y por CPU.
- Diseño del programa y recursos de ejecución.

---

## Punto de partida: varios hilos

```text
Proceso
├── Hilo A → interfaz
├── Hilo B → red
└── Hilo C → almacenamiento
```

Tener varias secuencias no indica, por sí solo, cuántas se ejecutan simultáneamente.

---

## Concurrencia

> **Varias tareas pueden progresar durante el mismo periodo de tiempo.**

Modelo de una unidad de procesamiento:

```text
Tiempo →
CPU: | A | B | A | C | B | A | C |
```

Las tareas avanzan mediante **intercalado**.

Un editor puede alternar trabajo de interfaz, red y autoguardado.

---

## Paralelismo

> **Varias tareas se ejecutan realmente en el mismo instante.**

```text
Tiempo →
Núcleo 1: | A | A | A | A |
Núcleo 2: | B | B | B | B |
Núcleo 3: | C | C | C | C |
```

Requiere unidades de procesamiento capaces de trabajar simultáneamente.

---

## Comparación directa

| Aspecto | Concurrencia | Paralelismo |
|---|---|---|
| Idea | Varias tareas en progreso | Ejecución simultánea |
| Una unidad de procesamiento | Es posible | No en este modelo |
| Mecanismo ilustrado | Intercalado | Varios núcleos |
| Ejemplo | Peticiones con esperas | Imágenes procesadas a la vez |

**Pueden coexistir en una misma aplicación.**

---

## Procesos e hilos no son núcleos

```text
3 hilos, 1 unidad:
CPU: A → B → C → A → B → C

3 hilos, 3 unidades capaces de ejecutarlos:
CPU 1: A → A → A
CPU 2: B → B → B
CPU 3: C → C → C
```

**Crear hilos no garantiza ejecución paralela.**

---

## Secuencial, concurrente y paralelo

```text
Secuencial:
CPU: | A | A | B | B | C | C |

Concurrente, por intercalado:
CPU: | A | B | C | A | B | C |

Paralelo:
CPU 1: | A | A |
CPU 2: | B | B |
CPU 3: | C | C |
```

En el primer caso, cada tarea termina antes de iniciar la siguiente.

---

## Servidor web

```text
Secuencial:
Petición A completa → Petición B completa

Concurrente:
A espera BD → B avanza → A continúa

Paralelo:
Núcleo 1 procesa A
Núcleo 2 procesa B   ← mismo instante
```

> ¿Qué limita cada escenario: las esperas, la CPU o ambos?

---

## Trabajo con mucha espera

Ejemplos:

- Consultas a una base de datos remota.
- Llamadas a una API.
- Descargas y operaciones de almacenamiento.

```text
Iniciar operación → esperar → procesar respuesta
```

La concurrencia permite aprovechar la espera para gestionar otras tareas.

---

## Trabajo con mucho cálculo

Ejemplos:

- Procesamiento de imágenes.
- Compresión.
- Análisis de datos.
- Compilación de unidades independientes.

El paralelismo puede distribuir cálculo entre unidades disponibles.

---

## Cuatro imágenes

Cada imagen requiere **4 s de CPU** en el modelo propuesto.

```text
Una unidad:
Imagen 1 → Imagen 2 → Imagen 3 → Imagen 4
          ≈ 16 s de cálculo total

Cuatro unidades:
Núcleo 1 → imagen 1
Núcleo 2 → imagen 2
Núcleo 3 → imagen 3
Núcleo 4 → imagen 4
          ≈ 4 s en condiciones ideales
```

Alternar tareas en una sola unidad no multiplica su capacidad de cálculo.

---

## Tres consultas de red

Cada consulta tarda aproximadamente **2 s**, principalmente esperando.

```text
Secuencial: A [2 s] → B [2 s] → C [2 s]   ≈ 6 s

Concurrente:
A [── 2 s ──]
B [── 2 s ──]
C [── 2 s ──]                            ≈ 2 s
```

Las barras incluyen espera. Los tiempos son aproximaciones con operaciones independientes y sobrecostes pequeños.

---

## Un servidor puede combinar ambos

```text
1000 clientes concurrentes
            ↓
      tareas disponibles
            ↓
         8 núcleos
```

Muchas solicitudes pueden estar en progreso mientras algunas ejecutan trabajo en paralelo.

**No hace falta un núcleo dedicado para cada cliente.**

---

## Diseño y ejecución

- La organización concurrente permite gestionar varias actividades.
- El paralelismo depende también del entorno de ejecución y el hardware.
- Más núcleos no aceleran automáticamente cualquier programa.
- Dependencias y sobrecostes limitan la mejora.

---

## Actividad visual

A y B son tareas informáticas. Cada columna representa el mismo instante.

```text
Caso 1
CPU:   A A B B A B

Caso 2
CPU 1: A A A A
CPU 2: B B B B

Caso 3
CPU 1: A A B B A A
CPU 2: C C D D C C
```

Identifica intercalado, concurrencia y paralelismo. Justifica con el diagrama.

---

## Otros ejemplos informáticos

| Aplicación | Trabajo |
|---|---|
| Antivirus | Analizar varios ficheros |
| Navegador | Descargar CSS e imágenes |
| Compilador | Compilar archivos independientes |

Para cada caso:

1. ¿Dónde puede haber espera?
2. ¿Dónde puede haber cálculo?
3. ¿Qué cambia al disponer de varios núcleos?

---

## Actividad: clasifica y justifica

1. Un script descarga un fichero y después el siguiente.
2. Cinco descargas progresan con una unidad de procesamiento.
3. Cuatro núcleos procesan cuatro imágenes simultáneamente.
4. Un servidor gestiona 500 peticiones y dispone de ocho núcleos.
5. Tres hilos se alternan sobre una unidad de procesamiento.

Distingue lo que afirma el enunciado de lo que solo sería posible.

---

## Caso práctico: servicio HTTP

```text
Recibir solicitudes
        ↓
Consultar BD
        ↓
Procesar imágenes
        ↓
Escribir logs
```

¿Qué partes pueden beneficiarse del solapamiento de esperas?

¿Qué partes podrían distribuir cálculo entre núcleos?

---

## Actividad final: procesamiento de documentos

Cuatro documentos deben pasar por:

```text
Descarga → Análisis → Guardado
   red        CPU       disco
```

Equipo con cuatro núcleos.

1. Identifica oportunidades de concurrencia.
2. Identifica oportunidades de paralelismo.
3. Dibuja una propuesta que respete las dependencias de cada documento.

---

## Resumen

```text
Mismo intervalo → tareas en progreso → CONCURRENCIA
Mismo instante  → ejecución simultánea → PARALELISMO
```

- Puede haber concurrencia mediante intercalado.
- Concurrencia y paralelismo pueden coexistir.
- La mejora depende de las tareas y de los recursos.

**Siguiente sesión:** crear y ejecutar hilos con Python.
