# ControlMotorOnOff

Sistema de control **Si-No (ON/OFF)** para un electroventilador utilizando
Arduino Uno y realimentación de velocidad mediante un sensor de RPM.

## Funcionamiento

Parámetros actuales definidos en `src/main.cpp`:

| Parámetro | Valor |
|---|---:|
| Velocidad objetivo | 1500 RPM |
| Histéresis | 200 RPM |
| Límite inferior | 1300 RPM |
| Límite superior | 1700 RPM |
| Período de muestreo | 100 ms |
| Tiempo inicial sin control | 5 s |

La lógica de control es:

- Si `RPM < 1300`, el motor se enciende.
- Si `RPM > 1700`, el motor se apaga.
- Entre ambos límites, conserva su estado anterior.

## Hardware y conexiones

El proyecto está configurado para Arduino Uno:

| Función | Pin Arduino |
|---|---:|
| Entrada del sensor de RPM | D2 |
| Salida de control del motor | D6 |
| Botón de parada de emergencia | D7 |
| LED indicador de parada | D13 |

### Sensor de RPM

La señal del sensor se conecta a `D2`, que admite interrupciones externas en el
Arduino Uno. El módulo `src/rpm.cpp` calcula las RPM a partir de los cambios de
estado de esa señal.

El valor `CAMBIOS_P_VUELTA` en `src/rpm.h` está configurado en `4`. Debe
coincidir con la cantidad real de cambios de estado producidos por cada vuelta
del electroventilador o por el elemento sensor utilizado.

### Electroventilador

El pin `D6` **no debe conectarse directamente al electroventilador**. Debe
conectarse a una etapa de potencia adecuada, por ejemplo un MOSFET, transistor,
relé o módulo de relé dimensionado para la tensión y corriente del ventilador.

La fuente del electroventilador debe ser independiente de la alimentación del
Arduino cuando la corriente del motor lo requiera. Las masas del Arduino, del
sensor y de la etapa de potencia deben compartir referencia cuando el circuito
lo necesite.

### Parada de emergencia

El botón está configurado con `INPUT_PULLUP` en `D7`. Al presionarlo, la entrada
queda en `LOW`, se apaga el motor y el sistema queda detenido en un estado
seguro hasta reiniciar el Arduino.

## Carga del firmware

1. Abrir el proyecto en PlatformIO.
2. Verificar las conexiones y los valores de `src/main.cpp` y `src/rpm.h`.
3. Compilar y cargar el firmware en el Arduino Uno.
4. Abrir el monitor serie a `9600` baudios para observar las RPM y el estado del
   motor.

El firmware actualmente imprime líneas con este formato:

```text
Tiempo[ms]: 1234 | RPM Actuales: 1450 | Estado Motor: ENCENDIDO
```

## Herramientas Python

Los scripts de la carpeta `PC` permiten guardar y graficar una prueba:

- `capturar.py`: lee el puerto serie y guarda las mediciones en un archivo TSV.
- `graficar.py`: genera un gráfico PNG con RPM, setpoint, histéresis y estado
  del motor.

### Instalación de dependencias

Desde PowerShell, ejecutar:

```powershell
py -m pip install pyserial numpy matplotlib
```

### Captura de datos

Cerrar el monitor serie antes de iniciar la captura, ya que el puerto COM no
puede ser utilizado por dos programas al mismo tiempo.

Desde la carpeta `PC`, reemplazar `COM5` por el puerto asignado al Arduino:

```powershell
py capturar.py COM5 datos_rpm.tsv
```

La captura continúa hasta presionar `Ctrl+C`.

### Generación del gráfico

Después de finalizar una captura:

```powershell
py graficar.py datos_rpm.tsv "Control Si-No del electroventilador"
```

El resultado se guarda como `datos_rpm.png` en la misma carpeta que el archivo
de datos.

Los archivos de mediciones y gráficos están excluidos del repositorio mediante
`.gitignore`, porque son resultados de pruebas y no forman parte del código
fuente.

## Estructura del proyecto

```text
ControlMotorOnOff/
├── PC/
│   ├── capturar.py
│   └── graficar.py
├── src/
│   ├── controlSiNo_sca.cpp
│   ├── controlSiNo_sca.h
│   ├── main.cpp
│   ├── rpm.cpp
│   └── rpm.h
├── platformio.ini
└── README.md
```
