#!/usr/bin/env python3
# =====================================================================
# Archivo:     graficar.py  
# Breve:       Graficación de los datos adquiridos utilizado en 
#              Sistemas de Control Automático (UNDAV).
# Autor:       Tomás Domancich
# Fecha:       Junio 2026
# 
# Descripción: 
#   Lee un TSV generado por capturar.py y grafica:
#    - RPM medida y setpoint
#    - Límites de histéresis
#    - Estado del motor (encendido/apagado)
#   Exporta automáticamente un PNG con el mismo nombre del TSV.
#
# Uso:
#    python graficar.py archivo.tsv ["Título del gráfico"]
#
# Ejemplos:
#   python graficar.py datos_rpm.tsv "Control Si-No del electroventilador"
#
# Requiere:    pip install numpy matplotlib
# =====================================================================

import sys
import os
import numpy as np
import matplotlib.pyplot as plt

SETPOINT_DEFAULT = 1500.0  # RPM, usado si el archivo no contiene setpoint
HISTERESIS = 200.0         # RPM, debe coincidir con main.cpp


def cargar(nombre):
    d = np.genfromtxt(nombre, delimiter="\t", names=True, dtype=None,
                      encoding="utf-8", ndmin=1)
    columnas = d.dtype.names or ()
    requeridas = {"Tiempo_ms", "RPM", "EstadoMotor"}
    faltantes = requeridas.difference(columnas)
    if faltantes:
        raise ValueError(
            "Faltan columnas en el archivo: " + ", ".join(sorted(faltantes))
        )

    t_min = d["Tiempo_ms"].astype(float) / 60000.0
    rpm = d["RPM"].astype(float)
    estado = d["EstadoMotor"].astype(float)

    if "Setpoint" in columnas:
        setpoint = d["Setpoint"].astype(float)
        if np.all(np.isnan(setpoint)):
            setpoint = np.full_like(rpm, SETPOINT_DEFAULT)
    else:
        setpoint = np.full_like(rpm, SETPOINT_DEFAULT)

    return t_min, rpm, setpoint, estado


def graficar(csv, titulo):
    t, rpm, setpoint, estado = cargar(csv)

    fig, (ax1, ax2) = plt.subplots(
        2, 1, figsize=(8, 6), dpi=110, sharex=True,
        gridspec_kw={"height_ratios": [3, 1]},
    )

    # --- Velocidad ---
    ax1.plot(t, rpm, color="#1769aa", lw=1.1, label="RPM medida")
    ax1.plot(t, setpoint, color="black", ls="--", lw=1.0, label="Setpoint")
    ax1.plot(t, setpoint + HISTERESIS, color="gray", ls=":", lw=0.9,
             label="Límites de histéresis")
    ax1.plot(t, setpoint - HISTERESIS, color="gray", ls=":", lw=0.9)
    ax1.set_ylabel("Velocidad [RPM]")
    ax1.set_title(titulo)
    ax1.legend(loc="lower right", fontsize=8)
    ax1.grid(True, alpha=0.3)

    # --- Estado del motor ---
    ax2.step(t, estado, where="post", color="green", lw=1.0)
    ax2.set_yticks([0, 1])
    ax2.set_yticklabels(["Apagado", "Encendido"])
    ax2.set_ylabel("Motor")
    ax2.set_xlabel("Tiempo [min]")
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    salida = os.path.splitext(csv)[0] + ".png"
    plt.savefig(salida, facecolor="white")
    print(f"# Gráfico guardado en {salida}")


def main():
    if len(sys.argv) < 2:
        print('Uso: python graficar.py archivo.tsv ["Título"]')
        sys.exit(1)
    csv = sys.argv[1]
    titulo = sys.argv[2] if len(sys.argv) > 2 else os.path.basename(csv)
    graficar(csv, titulo)


if __name__ == "__main__":
    main()
