#ifndef DEFAULTS_H
#define DEFAULTS_H



//TIMERS
#define SENSORS_INTERVAL        10 //milliseconds
#define TELEMETRY_INTERVAL      50 //milliseconds
#define VENTILATOR_INTERVAL     100 //milliseconds
#define COMMUNICATIONS_INTERVAL 500 //milliseconds
//------HardWare---------
#define ENTRY_EV_PIN    6   //Valvula - Entrada
#define EXIT_EV_PIN     11  //Valvula - Salida
#define ENTRY_FLOW_PIN  3   //Sensor de Flujo - Entrada
#define EXIT_FLOW_PIN   4   //Sendor de Flujo - Salida

#if defined(ARDUINO_ARCH_ESP32)
  #define STEPPER1_ENABLE  0
  #define STEPPER1_DIR     0
  #define STEPPER1_STEP    0
  #define STEPPER1_ENDSTOP 0
    //RAMPS_Y STEPER y ENDSTOP Y_MIN
  #define STEPPER2_ENABLE  0
  #define STEPPER2_DIR     0
  #define STEPPER2_STEP    0
  #define STEPPER2_ENDSTOP 0
#else
  //Sttepers (if needed)
    //RAMPS_X STEPER y ENDSTOP X_MIN
  #define STEPPER1_ENABLE  38
  #define STEPPER1_DIR     A1
  #define STEPPER1_STEP    A0
  #define STEPPER1_ENDSTOP  3
    //RAMPS_Y STEPER y ENDSTOP Y_MIN
  #define STEPPER2_ENABLE  A2
  #define STEPPER2_DIR     A7
  #define STEPPER2_STEP    A6
  #define STEPPER2_ENDSTOP 14
#endif
//#define PRESSURE_SENSOR_PIN      ??
//-------I2C--------
#define BME280_ADDR 0x76
#define BME280_ADDR_OFFSET 0x77
#define LCD_DIR     0x27 //I2C Address Display
// ---- Encoders ----
//RPM
#define PIN_ENC_RPM_DT  47
#define PIN_ENC_RPM_CLK 32
#define PIN_ENC_RPM_SW  0
// % insuflacion
#define PIN_ENC_PCTINS_DT   43
#define PIN_ENC_PCTINS_CLK  45
#define PIN_ENC_PCTINS_SW   0
// Vol Tidal
#define PIN_ENC_TIDAL_DT  39
#define PIN_ENC_TIDAL_CLK 41
#define PIN_ENC_TIDAL_SW  0
// Pause button
#define PIN_PAUSE 99

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
#define NO_PIN 0
/*
#if __has_include("myhacks.h")
# include "myhacks.h"
#endif
*/
#endif
