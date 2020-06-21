#ifndef DEFAULTS_H
#define DEFAULTS_H
#include "Arduino.h"


//TIMERS
#define SENSORS_INTERVAL        10 //milliseconds
#define TELEMETRY_INTERVAL      50 //milliseconds
#define VENTILATOR_INTERVAL     100 //milliseconds
#define COMMUNICATIONS_INTERVAL 500 //milliseconds

//------HardWare---------
#define NO_PIN -1
#define ENTRY_EV_PIN    NO_PIN   //Valvula - Entrada
#define EXIT_EV_PIN     NO_PIN  //Valvula - Salida
#define ENTRY_FLOW_PIN  NO_PIN   //Sensor de Flujo - Entrada
#define EXIT_FLOW_PIN   NO_PIN   //Sendor de Flujo - Salida

//STEPPER 1 CONFIG

#ifndef STEPPER1_ENABLE //ENABLE pin
  #define STEPPER1_ENABLE -1
#endif

#ifndef STEPPER1_ENABLE_STATE
  #define STEPPER1_ENABLE_STATE LOW
#endif

#ifndef STEPPER1_DIR
  #define STEPPER1_DIR    -1
#endif

#ifndef STEPPER1_STEP
  #define STEPPER1_STEP   -1
#endif

#ifndef STEPPER1_ENDSTOP
  #define STEPPER1_ENDSTOP -1
#endif

#ifndef STEPPER1_ENDSTOP_PRESSED_STATE
  #define STEPPER1_ENDSTOP_PRESSED_STATE HIGH
#endif

//comentar/descomentar segun se necesite
#define STEPPER1_ENDSTOP_PULLUP

#ifndef STEPPER1_MAX_RPM
  #define STEPPER1_MAX_RPM 200
#endif

#ifndef STEPPER1_STEPS_PER_RPM
  #define STEPPER1_STEPS_PER_RPM 160
#endif

#ifndef STEPPER1_MICROSTEPS
  #define STEPPER1_MICROSTEPS 8
#endif

#ifndef STEPPER1_OPEN_POS
  #define STEPPER1_OPEN_POS 200
#endif

#ifndef STEPPER1_CLOSED_POS
  #define STEPPER1_CLOSED_POS 575
#endif

////////// STEPPER 2 CONFIG
#ifndef STEPPER2_ENABLE //ENABLE pin
  #define STEPPER2_ENABLE -1
#endif

#ifndef STEPPER2_ENABLE_STATE
  #define STEPPER2_ENABLE_STATE LOW
#endif

#ifndef STEPPER2_DIR
  #define STEPPER2_DIR    -1
#endif

#ifndef STEPPER2_STEP
  #define STEPPER2_STEP   -1
#endif

#ifndef STEPPER2_ENDSTOP
  #define STEPPER2_ENDSTOP -1
#endif

#ifndef STEPPER2_ENDSTOP_PRESSED_STATE
  #define STEPPER2_ENDSTOP_PRESSED_STATE HIGH
#endif

//comentar/descomentar segun se necesite
#define STEPPER2_ENDSTOP_PULLUP

#ifndef STEPPER2_MAX_RPM
  #define STEPPER2_MAX_RPM 12
#endif

#ifndef STEPPER2_STEPS_PER_RPM
  #define STEPPER2_STEPS_PER_RPM 5400
#endif

#ifndef STEPPER2_MICROSTEPS
  #define STEPPER2_MICROSTEPS 2
#endif

#ifndef STEPPER2_OPEN_POS
  #define STEPPER2_OPEN_POS 700
#endif

#ifndef STEPPER2_CLOSED_POS
  #define STEPPER2_CLOSED_POS 1690
#endif


//#define PRESSURE_SENSOR_PIN      ??
//-------I2C--------
#define BME280_ADDR 0x76
#define BME280_ADDR_OFFSET 0x77
#define LCD_DIR     0x27 //I2C Address Display

// Pause button
#define PIN_PAUSE 99

// CONFIG POR DEFECTO
#define DEFAULT_RPM        15
#define DEFAULT_PMAX       50
#define DEFAULT_PPEAK      25
#define DEFAULT_PPEEP       5
#define DEFAULT_VTIDAL    450
#define DEFAULT_IERATIO     0.33
#define DEFAULT_IPAUSERATIO 0.50


//VALORES PARA LA CALIBRACION
  //Presion baja y alta
  #define CALIBRATION_LOW_PRESSURE   5
  #define CALIBRATION_HIGH_PRESSURE  35
  //Valores minimos y maximos de volumen para la tabla de calibracion
  #define CALIBRATION_TABLE_RESOLUTION     10
  #define CALIBRATION_TABLE_MIN_VOLUME    1.0
  #define CALIBRATION_TABLE_MAX_VOLUME  100.0
  //Volumen maquina+tubos
  #define CALIBRATION_TUBES_VOLUME_LOW   1.5
  #define CALIBRATION_TUBES_VOLUME_HIGH  1.6
  //Volumen dispositivo de calibracion
  #define CALIBRATION_BALLON_VOLUME_LOW  0.1
  #define CALIBRATION_BALLON_VOLUME_HIGH 0.6

// Valores por defecto
#define DEFAULT_ESTATURA 170 // cm
#define DEFAULT_SEXO       0 // 0: varón, 1: mujer
#define DEFAULT_ML_POR_KG_DE_PESO_IDEAL 7
#define DEFAULT_MAX_VOLUMEN_TIDAL 800
#define DEFAULT_MIN_VOLUMEN_TIDAL 240
#define DEFAULT_FLUJO_TRIGGER 3
#define DEFAULT_RPM     15
#define DEFAULT_MAX_RPM 30
#define DEFAULT_MIN_RPM 10
#define DEFAULT_POR_INSPIRATORIO 33 // %
#define DEFAULT_CMH2O_FLUX_TRIGGER_VALUE -10

#define DEFAULT_CMH20_MAX     50  //Presión máxima de seguridad
#define DEFAULT_CMH20_PICO    45  //Presión máxima regular
#define DEFAULT_CMH20_MESETA  32  //Presión mínima al insuflar
#define DEFAULT_CMH20_PEEP    14  //Presión mínima en el vaciado


#define DEFAULT_WEIGHT 80
#define DEFAULT_HEIGHT 170
#define DEFAULT_GENDER 0
#define DEFAULT_POR_FIO2 90

#define MAX_STEPPER_ERROR 0.05 //Error percent alowed when testing sttepers
#define PIN_BUZZER 53

#define SERIAL_BAUDS 921600

#endif
