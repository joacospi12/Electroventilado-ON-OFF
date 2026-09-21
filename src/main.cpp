#include <Arduino.h>
#include "controlSiNo_sca.h"
#include "rpm.h"

// ----- PINES -----
const int pinSensor = 2;  // Pin que soporta interrupciones externas (D2)
const int pinMotor = 6;   // Pin de salida al motor
const int pinBoton = 7;   // Pin para el botón de parada de emergencia
const int pinLed = 13;    // Pin para el LED indicador de parada

// --------- PARÁMETROS ---------
#define objetivoRPM 1500.0   // RPM
#define histeresis 200.0     // RPM
#define DELTA_T 100          // milisegundos de muestreo
#define TIEMPO_ARRANQUE 5000 // 5 segundos de espera antes de controlar

// --------- VARIABLES GLOBALES ---------
unsigned long tiempo_actual = 0;
unsigned long tiempo_anterior = 0;
unsigned long tiempo_inicio = 0;
float rpm_objetivo = objetivoRPM;
float rpm_medida = 0.0;
bool estadoMotor = false; // Reemplaza la variable local que tenías

// Instanciamos el objeto del controlador
controlSiNo controladorMotor(pinMotor);

// --------- PROTOTIPOS ---------
bool debo_muestrear();
float medir();
void controlar();
void MostrarDatos();
void mostrar_encabezado();
bool boton_parada_presionado();
void parada();

//--------------- SETUP -------------------
void setup() {
  Serial.begin(9600);
  
  // Configuración de pines para el botón y LED de emergencia
  pinMode(pinBoton, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
  
  // Configuro la medición de velocidad pasando el pin del sensor
  ConfigurarRPM(pinSensor);
  
  // Configuro el control Si-No
  controladorMotor.Configurar(rpm_objetivo, histeresis, SALIDA_NORMAL);
  
  // Establecer tiempo inicial y mostrar info
  tiempo_inicio = millis();
  mostrar_encabezado();
  
  Serial.println("----- Sistema iniciado -----");
}

//--------------- LOOP -------------------

void loop() {
  // Parada de emergencia -------------
  if (boton_parada_presionado()) {
    parada();
  }

  //  Muestreo periódico ---------------
  if (debo_muestrear()) {
    
    // Medimos y actualizamos la variable global rpm_medida
    medir();
    
    // Controlamos el motor en base a la medición
    controlar();
    
    // Mostramos los datos ---------
    MostrarDatos();
  }
}

//---------------- DEBO MUESTREAR ------------------

bool debo_muestrear() {
  tiempo_actual = millis();
  if (tiempo_actual - tiempo_anterior >= DELTA_T) {
    tiempo_anterior = tiempo_anterior + DELTA_T;
    return true;
  }
  return false;
}

//--------------- MEDIR RPM -------------------

float medir() {
  rpm_medida = MedirRPM();
  return rpm_medida; 
}

//--------------- CONTROLAR MOTOR -------------------

void controlar() {
  // Actúa solo si ya pasó el tiempo de arranque seguro
  if (millis() - tiempo_inicio >= TIEMPO_ARRANQUE) {
    estadoMotor = controladorMotor.Controlar(rpm_medida);
  } else {
    estadoMotor = false;
  }
}

//--------------- PARADA DE EMERGENCIA -------------------

bool boton_parada_presionado() {
  // Retorna true si el botón (configurado como PULLUP) es presionado (LOW)
  return digitalRead(pinBoton) == LOW;
}

//--------------- PARADA DE EMERGENCIA -------------------

void parada() {
  // Apagar motor inmediatamente. 
  // Nota: Si la clase controlSiNo no tiene Apagar(), usa digitalWrite(pinMotor, LOW);
  controladorMotor.Apagar(); 
  
  Serial.println();
  Serial.println("============================================");
  Serial.println("       ¡PARADA DE EMERGENCIA ACTIVADA!      ");
  Serial.println("============================================");

  // Desactivar interrupción del sensor para dejar de medir
  detachInterrupt(digitalPinToInterrupt(pinSensor));

  // Entrar en un estado seguro (bucle infinito)
  while (true) {
    digitalWrite(pinLed, HIGH);
    // El sistema se queda bloqueado aquí hasta que se reinicie el Arduino
  }
}

//--------------- MOSTRAR DATOS ENCABEZADO -------------------

void mostrar_encabezado() {
  Serial.println("============================================");
  Serial.println("CONTROL DE RPM...");
  Serial.println("============================================");
  Serial.println("Datos: ");
  Serial.print  ("   RPM objetivo: ");
  Serial.print  (rpm_objetivo);
  Serial.println(" RPM");
  Serial.print  ("   Histéresis: ±");
  Serial.print  (histeresis);
  Serial.println(" RPM");
  Serial.println("--------------------------------------------");
  Serial.println();
}

//--------------- MOSTRAR DATOS ACTUALES -------------------

void MostrarDatos() {
  // Esta función mantiene tu formato original pero lee las variables globales
  Serial.print("Tiempo[ms]: ");
  Serial.print(millis());
  Serial.print(" | RPM Actuales: ");
  Serial.print(rpm_medida);
  Serial.print(" | Estado Motor: ");
  Serial.println(estadoMotor ? "ENCENDIDO" : "APAGADO");
}