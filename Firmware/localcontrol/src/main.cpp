/**
 * Name:     Local Control Ventilator -> LCD I2C + Encoder
 * Autor:    Víctor García Pastor
 * License:  GNU GP V3
 * Date:     17/04/2020
 *
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "../include/local.env.h"
#include "../include/defaults.h"
#include "menu.h"
#include "ApolloConnect.h"

ApolloConnect *connect = new ApolloConnect(115200);

itemmenu_t mmode = {"Mode",Screen::WorkMode,connect->_mode,0,1,3};
itemmenu_t mweight = {"Weight",Screen::Number,connect->_weight,10.0,1.0,150.0};
itemmenu_t mheight = {"Height",Screen::Number,connect->_height,10.0,1.0,250.0};
itemmenu_t msexo = {"Genere",Screen::Number,connect->_sexo,0,1,1};
itemmenu_t mrpm = {"RPM",Screen::Number,connect->_rpm,10,1,30};
itemmenu_t mtidal = {"Ml Tidal",Screen::Number,connect->_mlTidalVolume,240,1,800};
itemmenu_t mpctinsp = {"% Insp",Screen::Number,connect->_porcentajeInspiratorio,25,1,50};
itemmenu_t mpctfio2 = {"% FiO2",Screen::Number,connect->_porcentajeFiO2,80,1,100};
itemmenu_t mprespeep = {"Press. Peep",Screen::Number,connect->_pressionPeep,0,1,30};
itemmenu_t mprespico = {"Press. Pico",Screen::Number,connect->_pressionPico,20,1,50};
itemmenu_t mpresmeseta = {"Press. Meseta",Screen::Number,connect->_pressionMeseta,20,1,50};
itemmenu_t mpresmax = {"Press. Max",Screen::Number,connect->_pressionMax,20,1,60};
itemmenu_t mprestrigger = {"Trigger",Screen::Number,connect->_presionTriggerInspiration,-20,1,10};

ClickEncoder *encoder= new ClickEncoder(A1, A0, A2);
LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27,20,4);
Menu menu(lcd,encoder);

void timerIsr() {
  encoder->service();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("inicia config");

    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    
    lcd->begin(20, 4);
    lcd->init();
    lcd->backlight();
    lcd->clear();
    static char bARROW[]   = {                               // Bits icono flecha
        B00000, B00100, B00110, B11111,
        B00110, B00100, B00000, B00000
    };
    lcd->createChar(0, bARROW);
    lcd->setCursor(0,0);
    lcd->print("Inicio control");

    menu.begin(20,4);
    menu.addItem(mmode);
    menu.addItem(mweight);
    menu.addItem(mheight);
    menu.addItem(msexo);
    menu.addItem(mrpm);
    menu.addItem(mtidal);
    menu.addItem(mpctinsp);
    menu.addItem(mpctfio2);
    menu.addItem(mprespeep);
    menu.addItem(mprespico);
    menu.addItem(mpresmeseta);
    menu.addItem(mpresmax);
    menu.addItem(mprestrigger);

    Serial.println("EndConfig");
}

bool updated = true;
void loop()
{
    if(updated){
        menu.print();
        connect->send();
        updated = !updated;
    }
    updated = menu.update();
}
