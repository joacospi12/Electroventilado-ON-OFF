#!/usr/bin/env python3
# =============================================================================
# Archivo:     capturar.py  
# Breve:       Adquisición de datos por puerto serie utilizado en 
#              Sistemas de Control Automático (UNDAV).
# Autor:       Tomás Domancich
# Fecha:       Junio 2026
# Descripción:
#   Lee las líneas que el Arduino emite por Serial y las guarda en un TSV
#   normalizado con tiempo, RPM, setpoint y estado del motor.
#
# Uso:
#   python capturar.py [puerto] [archivo_salida.tsv]
#
# Ejemplos:
#   python capturar.py COM5 datos_rpm.tsv       (Windows)
#   python capturar.py /dev/ttyUSB0 datos_rpm.tsv (Linux)
#
# Requiere:    pip install pyserial
# =============================================================================

import sys
import time
import re
import serial   # pyserial

# ---- Parámetros por defecto ----
PUERTO_DEFAULT = "COM5"           # cambiar por el COM asignado al Arduino
BAUDIOS        = 9600              # debe coincidir con Serial.begin(9600)
SALIDA_DEFAULT = "datos_rpm.tsv"
SETPOINT       = 1500.0            # debe coincidir con objetivoRPM en main.cpp

ENCABEZADO = "Tiempo_ms\tRPM\tSetpoint\tEstadoMotor"

# Salida actual de main.cpp:
# Tiempo[ms]: 1234 | RPM Actuales: 1450 | Estado Motor: ENCENDIDO
PATRON_TEXTO = re.compile(
    r"Tiempo\[ms\]:\s*([0-9]+).*?RPM Actuales:\s*([-+0-9.]+).*?"
    r"Estado Motor:\s*(ENCENDIDO|APAGADO)",
    re.IGNORECASE,
)


def convertir_linea(linea):
    """Devuelve una medición normalizada o None si no es una línea de datos."""
    partes = linea.split("\t")

    # Permite usar directamente el formato TSV recomendado en el firmware.
    if len(partes) >= 4:
        try:
            tiempo = int(float(partes[0]))
            rpm = float(partes[1])
            setpoint = float(partes[2])
            estado = int(float(partes[3]))
            if estado not in (0, 1):
                return None
            return f"{tiempo}\t{rpm:g}\t{setpoint:g}\t{estado}"
        except ValueError:
            pass

    # También acepta la salida textual que genera actualmente main.cpp.
    coincidencia = PATRON_TEXTO.search(linea)
    if coincidencia:
        tiempo, rpm, estado = coincidencia.groups()
        estado_num = 1 if estado.upper() == "ENCENDIDO" else 0
        return f"{int(tiempo)}\t{float(rpm):g}\t{SETPOINT:g}\t{estado_num}"

    return None

def main():
    puerto = sys.argv[1] if len(sys.argv) > 1 else PUERTO_DEFAULT
    salida = sys.argv[2] if len(sys.argv) > 2 else SALIDA_DEFAULT

    print(f"# Abriendo {puerto} @ {BAUDIOS} baudios...")
    try:
        ser = serial.Serial(puerto, BAUDIOS, timeout=2)
    except serial.SerialException as e:
        print(f"ERROR: no se pudo abrir {puerto}: {e}")
        sys.exit(1)

    time.sleep(2)          # espera al reset del ESP32 al abrir el puerto
    ser.reset_input_buffer()

    print(f"# Guardando en {salida}  (Ctrl+C para terminar)")
    n = 0
    with open(salida, "w", encoding="utf-8") as f:
        f.write(ENCABEZADO + "\n")
        try:
            while True:
                linea = ser.readline().decode("utf-8", errors="ignore").strip()
                if not linea:
                    continue
                medicion = convertir_linea(linea)
                if medicion is None:
                    # Los mensajes de inicio y emergencia no son mediciones.
                    print(f"  {linea}")
                    continue

                f.write(medicion + "\n")
                f.flush()
                n += 1
                print(f"[{n:5d}] {medicion}")
        except KeyboardInterrupt:
            print(f"\n# Detenido por el usuario. {n} muestras guardadas en {salida}.")
        finally:
            ser.close()

if __name__ == "__main__":
    main()
