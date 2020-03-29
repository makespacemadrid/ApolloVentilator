### CURRENT_DIR
### Do not touch - used for binaries path
CURRENT_DIR       = $(shell basename $(CURDIR))

ARDMK_DIR         := /opt/compilation

### PROJECT_DIR
### This is the path to where you have created/cloned your project
ARDUINO_SKETCHBOOK = ./
ARDUINO_DIR       = /opt/arduino
ARDUINO_LIBS      = LiquidCrystal LiquidCrystal_I2C Wire AccelStepper

PROJECT_DIR       = /app
USER_LIB_PATH     = $(PROJECT_DIR)/src
ARDUINO_LIB_PATH  = $(ARDUINO_DIR)/hardware/arduino/avr/libraries/
ARCHITECTURE      = avr
BOARD_TAG         ?= uno
MONITOR_BAUDRATE  = 115200

AVRDUDE          = /usr/bin/avrdude

### CFLAGS_STD
### Set the C standard to be used during compilation. Documentation (https://github.com/WeAreLeka/Arduino-Makefile/blob/std-flags/arduino-mk-vars.md#cflags_std)
CFLAGS_STD        = -std=gnu11

### CXXFLAGS_STD
### Set the C++ standard to be used during compilation. Documentation (https://github.com/WeAreLeka/Arduino-Makefile/blob/std-flags/arduino-mk-vars.md#cxxflags_std)
CXXFLAGS_STD      = -std=gnu++11

### OBJDIR
### This is where you put the binaries you just compile using 'make'
OBJDIR            = $(PROJECT_DIR)/bin/$(BOARD_TAG)

### Do not touch - the path to Arduino.mk, inside the ARDMK_DIR
include $(ARDMK_DIR)/Arduino.mk
