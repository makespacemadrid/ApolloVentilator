#ifndef DEFAULTS_H
#define DEFAULTS_H

#define LOG_INTERVAL 100 //milliseconds

//------HardWare---------
#define ENTRY_EV_PIN 10 //ElectroValvula - Entrada  - Soplador de Aire
//#define ENTRY_EV_PIN    9 //ElectroValvula - Entrada    - Compresor
#define EXIT_EV_PIN 8    //ElectroValvula - Salida
#define ENTRY_FLOW_PIN 3 //Sensor de Flujo - Entrada
#define EXIT_FLOW_PIN 4  //Sendor de Flujo - Salida
//#define PRESSURE_SENSOR_PIN      ??
//-------I2C--------
#define BME280_ADDR 0x76
#define LCD_DIR 0x27 //I2C Address Display
// ---- Encoders ----
//RPM
#define PIN_ENC_RPM_DT 47
#define PIN_ENC_RPM_CLK 32
#define PIN_ENC_RPM_SW 0
// % insuflacion
#define PIN_ENC_PCTINS_DT 43
#define PIN_ENC_PCTINS_CLK 45
#define PIN_ENC_PCTINS_SW 0
// Vol Tidal
#define PIN_ENC_TIDAL_DT 39
#define PIN_ENC_TIDAL_CLK 41
#define PIN_ENC_TIDAL_SW 0

// Valores por defecto
#define DEFAULT_ESTATURA 170 // cm
#define DEFAULT_SEXO 0       // 0: varón, 1: mujer
#define DEFAULT_ML_POR_KG_DE_PESO_IDEAL 7
#define DEFAULT_MAX_VOLUMEN_TIDAL 800
#define DEFAULT_MIN_VOLUMEN_TIDAL 240
#define DEFAULT_FLUJO_TRIGGER 3
#define DEFAULT_RPM 15
#define DEFAULT_MAX_RPM 30
#define DEFAULT_MIN_RPM 10
#define DEFAULT_POR_INSPIRATORIO 33 // %
#define DEFAULT_CMH2O_FLUX_TRIGGER_VALUE -10
#define DEFAULT_CMH20_PICO 45
#define DEFAULT_CMH20_MESETA 32
#define DEFAULT_CMH20_PEEP 14
#define DEFAULT_CMH20_MAX 50

#define DEFAULT_WEIGHT 80
#define DEFAULT_HEIGHT 170
#define DEFAULT_GENDER 0
#define DEFAULT_POR_FIO2 90
#endif
