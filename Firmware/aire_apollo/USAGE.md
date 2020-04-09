# Usage of firmware

This firmware has ben developemt using Platform IO

## Requirements
To deploy the firmware it's neccesary the next components

- [ ] Arduino Mega
- [ ] Pressure sensor
- [ ] Two valves
- [ ] Two flow sensors

If you like to enable Local Controls can add the nest components
- [ ] Display
- [ ] 3X Encoders
- [ ] Button

## Usage of firmware
1. Clone the repo and checkout the last tag
2. Edit platformio.ini if you like to use other board
3. Edit `include/defaults.h` file to update the pins that you are using
4. Edit `main.cpp` file and update the lines of initialitation of sensors and valves, with the class of your sensors and valves with the requirements parameters in constructor.

`ApolloFlowSensor *fInSensor   = new MksmFlowSensor();
  ApolloFlowSensor *fOutSensor  = new MksmFlowSensor();
  ApolloPressureSensor *pSensor = new mksBME280(BME280_ADDR);
  ApolloValve *inValve  = new StepperNema(STEPER1_ENABLE,STEPER1_DIR,STEPER1_STEP,0,0,2500,20);
  ApolloValve *outValve = new StepperNema(STEPER2_ENABLE,STEPER2_DIR,STEPER2_STEP,0,0,2500,20);
`

5. (Optional) Only if you have LocalControls enable it uncomment `#define LOCALCONTROLS`
