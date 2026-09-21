/**************************************************************************************************
* Módulo para control SI-NO
***************************************************************************************************
* Archivo:  controlSiNo_sca.h
* Resumen:  Librería de control si-no elaborada con programación orientada a objetos (POO)
*           para el curso de Sistemas de Control Automático, UNDAV.
* Autor:    Guillermo Caporaletti <gfcaporaletti@undav.edu.ar>
* Fecha:    2025
* Version:  2
* Cambios:  Restringe el constructor a configuraciones de hardware.
*           Modifica los parámetros de las funciones.
*           Versión anterior: 1.1 (2023).
*
**************************************************************************************************/

#ifndef CONTROL_SINO_SCA_H
#define CONTROL_SINO_SCA_H

#include "Arduino.h"

#define SALIDA_INVERTIDA    true
#define SALIDA_NORMAL       false

/*************************************************************************************************/

class controlSiNo                       // Objeto para control SI-NO.
{
  private:
    boolean EstadoSalida;               // Estado de salida del control SI-NO.
    byte    PinSalida;                  // Pin de salida. Si es 0, no actúa 
                                        // (y se debe actuar de otra forma).
    float   Superior;                   // OBJETIVO + HISTERESIS
    float   Inferior;                   // OBJETIVO - HISTERESIS
    bool    SalidaInvertida;            // --> false si apaga cuando MEDICION > Superior 
                                        //           y prende cuando MEDICION < Inferior
                                        // --> true  si apaga cuando MEDICION < Inferior
                                        //           y prende cuando MEDICION > Superior
  public:
    controlSiNo(uint8_t PIN);           // Constructor. Configura el pin de salida, si corresponde
                                        // Si el pin es 0, no modifica ningún pin
    void Configurar(float OBJETIVO, float HISTERESIS, bool MODO_SALIDA);
                                        // Configura el valor OBJETIVO y su HISTERESIS.
                                        // Permite establecer una SALIDA INVERTIDA o negada.
                                        // Es posible cambiar esta configuración durante la ejecución.
    void Configurar(float OBJETIVO);    // Configura sólo este parámetro.
    boolean Controlar(float MEDICION);  // Controla según medición, 
                                        //  modifica el estado del pin de salida 
                                        // y devuelve el estado del controlador. 
    void Apagar();                      // Pone EstadoSalida en false.
    void Prender();                     // Pone EstadoSalida en true.
    boolean Estado();                   // Devuelve el Estado del pin de salida.
};

/*************************************************************************************************/

#endif // CONTROL_SINO_H

/****************************************************************************** FIN DE ARCHIVO ***/
