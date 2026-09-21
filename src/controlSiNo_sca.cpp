/**************************************************************************************************
* Módulo para control SI-NO
***************************************************************************************************
* Archivo:  controlSiNo_sca.cpp
* Resumen:  Librería de control si-no. 
* Autor:    Guillermo Caporaletti <gfcaporaletti@undav.edu.ar>
*           (UNDAV, Ingeniería en Informática, Sistemas de Control Automático)
* Fecha:    2025
* Version:  2
**************************************************************************************************/

#include "Arduino.h"
#include "controlSiNo_sca.h"

/**************************************************************************************************
* Funciones públicas
**************************************************************************************************/

/**************************************************************************************************
* Función:  Constructor
* Resumen:  
**************************************************************************************************/
controlSiNo::controlSiNo(uint8_t PIN)
{
  EstadoSalida = false;               // Estado por defecto al iniciar.
  PinSalida = PIN;                    // Guarda pin de salida y configura si corresponde
  if (PinSalida>0) {                  // Configura salida en 0
    pinMode(PinSalida, OUTPUT);
    digitalWrite(PinSalida, EstadoSalida);
  }
  Configurar(0, 1, SALIDA_NORMAL);    // Configuración por defecto
}
/*************************************************************************************************/

/**************************************************************************************************
* Función:  Configurar
* Resumen:  Establece parámetros de control
**************************************************************************************************/
void controlSiNo::Configurar(float OBJETIVO, float HISTERESIS, bool MODO_SALIDA)
{
  HISTERESIS = abs(HISTERESIS);
  Superior   = OBJETIVO + HISTERESIS;
  Inferior   = OBJETIVO - HISTERESIS;
  if (true==MODO_SALIDA) {
    SalidaInvertida = true;
  } else {
    SalidaInvertida = false;
  }
}
/*************************************************************************************************/

/**************************************************************************************************
* Función:  Configurar (sobrecargada)
* Resumen:  Establece parámetros de control
**************************************************************************************************/
void controlSiNo::Configurar(float OBJETIVO)
{
  float HISTERESIS = (Superior - Inferior) / 2;
  Superior   = OBJETIVO + HISTERESIS;
  Inferior   = OBJETIVO - HISTERESIS;
}
/*************************************************************************************************/

/**************************************************************************************************
* Función:  Controlar
* Resumen:  Devuelve el estado del controlador luego de procesar la ENTRADA.
*           Si fue seteado un PIN de salida, cambia su estado.
**************************************************************************************************/
boolean controlSiNo::Controlar(float MEDICION)       
{
  if (MEDICION > Superior) { 
    // Se debe apagar (o prender si SalidaInvertida==true)
    if (SalidaInvertida) {
      EstadoSalida = true;
    } else {
      EstadoSalida = false;
    }
  } 
  if (MEDICION < Inferior) { 
    // Se debe prender (o apagar si SalidaInvertida==true)
    if (SalidaInvertida) {
      EstadoSalida = false;
    } else {
      EstadoSalida = true;
    }
  } 
  // ¿Se debe actuar en PinSalida?
  if (PinSalida>0) {
    digitalWrite(PinSalida, EstadoSalida);
  }
  // Fin
  return EstadoSalida;
}
/*************************************************************************************************/

/**************************************************************************************************
* Función:  Apagar()
* Resumen:  Pone en "false" el estado y apaga la salida PIN (si fue configurada).
**************************************************************************************************/
void controlSiNo::Apagar()
{
  EstadoSalida = false;
  if (PinSalida>0) digitalWrite(PinSalida, EstadoSalida);
}
/*************************************************************************************************/

/**************************************************************************************************
* Función:  Prender()
* Resumen:  Pone en "true" el estado y prende la salida PIN (si fue configurada).
**************************************************************************************************/
void controlSiNo::Prender()
{
  EstadoSalida = true;
  if (PinSalida>0) digitalWrite(PinSalida, EstadoSalida);
}
/*************************************************************************************************/

/**************************************************************************************************
* Función:  Estado()
* Resumen:  Devuelve el estado del controlador si-no.
**************************************************************************************************/
boolean controlSiNo::Estado()
{
  return EstadoSalida;
}
/*************************************************************************************************/

/****************************************************************************** FIN DE ARCHIVO ***/
