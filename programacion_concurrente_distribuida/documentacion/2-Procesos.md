# Sesión 2 — Procesos y funcionamiento del sistema operativo

**Programación Concurrente · Alumnos / Proyección**

> Si varios programas necesitan CPU, ¿cómo los organiza el sistema operativo?

---

## Qué vamos a comprender

- Programa frente a proceso.
- Memoria, recursos y aislamiento.
- Planificación, estados y cambio de contexto.
- Multiprogramación y reparto del tiempo.

---

## Del programa al proceso

```text
servidor.py → ejecución → PROCESO
```

**Programa:** instrucciones almacenadas.

**Proceso:** instancia de un programa en ejecución, con memoria y recursos asociados.

```text
               mismo programa
                /     |     \
          Proceso 1 Proceso 2 Proceso 3
```

Un programa puede originar varias ejecuciones independientes.

---

## ¿Qué necesita un proceso?

```text
Proceso A                Proceso B
├── código               ├── código
├── datos                ├── datos
├── espacio de memoria   ├── espacio de memoria
└── recursos             └── recursos
```

Un navegador puede utilizar varios procesos para sus componentes.

**Aplicación y proceso no siempre tienen una relación uno a uno.**

---

## Dos ejecuciones de Python

Archivo `programa1.py`:

```python
contador = 10
print(contador)
```

Ejecutado en dos terminales:

```text
Proceso A           Proceso B
contador = 10       contador = 10
```

Las variables no se comparten automáticamente entre procesos.

El **aislamiento** separa sus espacios de memoria.

---

## Muchos procesos, una CPU

Modelo: una unidad de procesamiento ejecuta una secuencia cada vez.

```text
P1 navegador ──┐
P2 editor ─────┤
P3 reproductor ┼──► Sistema operativo ──► CPU
P4 antivirus ──┤
P5 terminal ──┘
```

> ¿Es necesario que P1 termine para que P2 avance?

---

## Planificación

El **planificador** selecciona qué proceso preparado obtiene la CPU.

```text
Tiempo →
CPU: | P1 | P1 | P2 | P3 | P1 | P4 | P2 |
```

La CPU puede alternar entre procesos antes de que terminen.

---

## Multiprogramación y multitarea

| Modelo | Idea |
|---|---|
| Monoprogramación | Un programa atendido cada vez |
| Multiprogramación | Varios programas disponibles; se aprovechan las esperas |
| Multitarea | Las tareas progresan mediante reparto del procesador |

```text
A usa CPU → A espera disco → B usa CPU → B espera red
                                                   ↓
                                              C usa CPU
```

---

## Tres estados fundamentales

| Estado | Situación |
|---|---|
| **Preparado** | Puede avanzar, pero espera CPU |
| **Ejecución** | Está utilizando CPU |
| **Espera** | Necesita un evento o una operación pendiente |

**Preparado:** podría continuar si recibe CPU.

**Espera:** necesita primero que ocurra algo, por ejemplo recibir datos.

---

## Una fotografía del sistema

```text
Preparados         CPU           Espera
[P1, P4]           [P3]          [P2: red]
```

- P3 está ejecutando.
- P1 y P4 pueden competir por CPU.
- P2 necesita la respuesta de red.

> Si llegan los datos de P2, ¿obtiene necesariamente la CPU en ese instante?

---

## Transiciones de estado

```text
PREPARADO ── planificación ──► EJECUCIÓN
    ▲                              │
    │                              │ necesita E/S
    │                              ▼
    └── evento completado ───── ESPERA

EJECUCIÓN ── expiración del turno ──► PREPARADO
```

La creación incorpora una nueva ejecución; la terminación cierra su actividad.

Los términos **suspendido** e **inactivo** pueden aparecer en modelos más amplios; no son sinónimos universales de espera o preparado.

---

## Ejemplo: cargar una página

```text
Preparado
    ↓ recibe CPU
Ejecución: solicita la página
    ↓ espera respuesta
Espera de red
    ↓ llegan datos
Preparado
    ↓ recibe CPU
Ejecución: procesa HTML
```

El ciclo puede repetirse al solicitar imágenes y otros recursos.

---

## Ejemplo: guardar desde el editor

```text
Ctrl + S
    ↓
Prepara los datos usando CPU
    ↓
Solicita escritura y puede esperar
    ↓
Termina la E/S → preparado → ejecución
```

**Una aplicación abierta no utiliza CPU permanentemente.**

---

## Actividad: identifica el estado

1. Un proceso espera datos de Internet.
2. Un proceso calcula con la CPU.
3. Un proceso tiene trabajo, pero otro ocupa la CPU.
4. Un proceso termina una espera y aún no ha sido seleccionado.

Justifica cada elección.

---

## Cola de preparados y reparto temporal

```text
Cola de preparados → Planificador → CPU
        ▲                            │
        └──── fin del turno ─────────┘
```

Al expirar su turno, un proceso puede volver a preparado.

**Ceder la CPU no implica terminar el programa.**

---

## Contexto y cambio de contexto

El **contexto de ejecución** conserva el estado necesario para continuar.

```text
P1 ejecuta
    ↓ guardar contexto de P1
Restaurar contexto de P2
    ↓
P2 ejecuta
    ↓ más adelante
Restaurar P1 → P1 continúa
```

Cambiar de proceso tiene un coste: repartir CPU también requiere trabajo.

---

## Identificación de procesos: PID

**PID:** identificador de un proceso.

Abre el administrador de tareas o monitor de actividad y observa:

- Nombre y PID.
- Uso de CPU.
- Memoria utilizada.
- Procesos relacionados con una misma aplicación.

> ¿Todos los procesos muestran el mismo uso de CPU?

---

## Otras clasificaciones del sistema operativo

| Criterio | Tipos |
|---|---|
| Número de usuarios | Monousuario / multiusuario |
| Organización del trabajo | Por lotes / tiempo compartido / tiempo real |

- **Por lotes:** informes, copias de seguridad o procesamiento de logs.
- **Tiempo compartido:** reparto de recursos entre tareas o usuarios.
- **Tiempo real:** cumplir plazos de respuesta forma parte de los requisitos.

---

## Actividad: servidor de aplicaciones

La CPU va a seleccionar un proceso.

| Proceso | Situación |
|---|---|
| P1 · servidor web | Espera respuesta de red |
| P2 · base de datos | Consultas listas para procesar |
| P3 · logs | Espera una escritura en disco |
| P4 · monitorización | Cálculos pendientes |

1. Identifica los procesos en espera y los preparados.
2. ¿Entre cuáles puede elegir el planificador?
3. ¿Qué transición hará P1 al recibir la respuesta?

---

## Actividad: construye la línea temporal

```text
P1 navegador · P2 editor · P3 correo
```

1. P1 usa CPU y después espera la red.
2. P2 obtiene CPU; al acabar su turno vuelve a preparado.
3. P3 obtiene CPU.
4. Llega la respuesta de red de P1.
5. Más tarde P1 vuelve a obtener CPU.

Dibuja los estados de cada proceso durante toda la secuencia.

---

## Resumen

```text
Programa → Proceso → Memoria y recursos
                 ↓
       Preparado / Ejecución / Espera
                 ↓
      Planificación y cambio de contexto
```

- Un proceso puede existir sin estar usando CPU.
- Terminar una espera no garantiza ejecución inmediata.
- La CPU se reparte entre ejecuciones que pueden avanzar.

**Siguiente sesión:** varias líneas de ejecución dentro de un proceso: los hilos.
