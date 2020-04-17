#ifndef MENU_H
#define MENU_H

#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "Arduino.h"
#include "ClickEncoder.h"
#include <TimerOne.h>
#include "list.h"
#include "../include/defaults.h"


enum Button{
    Unknown,
    Left,
    Right,
    Pressed,
    Held,
    Released,
    Clicked,
    DoubleClicked
 } btnPressed;     // Enumerador con los diferentes botones disponibles

enum Screen{ WorkMode, Menu2, Flag, Number,Exit };
        const char *WorkModeTxt[] = {                              
            "Pressure",
            "Flow",
            "Pause"
        };
typedef struct{
    String title;
    Screen screen;
    double &value;
    double min;
    double step;
    double max;
} itemmenu_t;

class Menu{

    public:
        Menu(LiquidCrystal_I2C *lcd_,ClickEncoder *encoder_){
            encoder = encoder_;
            lcd = lcd_;
        };
        bool begin(uint8_t cols_, uint8_t rows_){
            Serial.println("IniConfig");
            this->lcdCols = cols_;
            this->lcdRows = rows_;
            this->menucompleto.Append(mexit);
            return true;
        };

        bool update(){
            btnPressed = readButtons();
            if( btnPressed == Button::Clicked )
                return openMenu();
            if( btnPressed == Button::Left || btnPressed == Button::Right)
                this->print(btnPressed);

            return false;
        };

        int addItem(itemmenu_t itemmenu_){
            this->menucompleto.DeleteLast();
            this->menucompleto.Append(itemmenu_);
            this->menucompleto.Append(mexit);
            return this->menucompleto.getLength();
        }
        void print(Button pos = Button::Unknown){
            this->lcd->clear();
            if(menucompleto.moveToStart()){
                if(pos==Button::Left){
                    if((this->printItem-1)<0){
                        this->printItem = 0;
                    }else{
                        this->printItem--;
                    }
                }
                if(pos ==Button::Right){
                    if((this->printItem+1)>(this->menucompleto.getLength()-lcdRows-2)){
                        this->printItem = (this->menucompleto.getLength()-lcdRows-2);
                    }else{
                        this->printItem++;
                    }
                }
                Serial.println("print "+String(this->printItem));
                
                int i = 0;
                int y = 0;
                do{
                    if(i>this->menucompleto.getLength()){
                        return;
                    }
                    Serial.println(String(y) + " " + String(i));
                    if(i<this->printItem || y>=lcdRows){
                    }else{
                        itemmenu_t itemmenu = this->menucompleto.getCurrent();
                        switch(itemmenu.screen){
                            case Screen::Exit:
                            break;
                            case Screen::WorkMode:
                                Serial.println((itemmenu.title)+"-->"+String(WorkModeTxt[int(itemmenu.value)]));
                                Serial.flush();
                                lcd->setCursor(0,y);
                                lcd->print((itemmenu.title)+"-->"+String(WorkModeTxt[int(itemmenu.value)]));
                                y++;
                            break;
                            default:
                                Serial.println((itemmenu.title)+"-->"+String((itemmenu.value)));
                                Serial.flush();
                                lcd->setCursor(0,y);
                                lcd->print((itemmenu.title)+"-->"+String((itemmenu.value)));
                                y++;
                            break;
                        }
                    }
                    i++;
                }while(menucompleto.next());
            }
        }
    private:
        
        LinkedList<itemmenu_t> menucompleto;
        uint8_t lcdRows;
        uint8_t lcdCols;
        ClickEncoder *encoder;
        LiquidCrystal_I2C *lcd; 
        byte iARROW     = 0;// ID icono flecha
        int16_t last, value;
        double exit;
        itemmenu_t mexit = {"Exit X",Screen::Exit,exit};

        int printItem = 0;


        void openSubMenuItem( itemmenu_t item )
        {
            boolean exitSubMenu = false;
            boolean forcePrint  = true;

            lcd->clear();

            while( !exitSubMenu )
            {
                btnPressed = readButtons();

                if( btnPressed == Button::Clicked )
                {
                    exitSubMenu = true;
                }
                else if( btnPressed == Button::Left && (item.value)-item.step >= item.min )
                {
                    item.value = (item.value)-item.step;
                }
                else if( btnPressed == Button::Right && (item.value)+item.step <= item.max )
                {
                    item.value = (item.value)+item.step;
                }

                if( !exitSubMenu && (forcePrint || btnPressed != Button::Unknown) )
                {
                    forcePrint = false;
                    lcd->clear();

                    lcd->setCursor(0,0);
                    lcd->print(String(item.title));

                    lcd->setCursor(0,1);
                    lcd->print("<");
                    lcd->setCursor(lcdCols-1,1);
                    lcd->print(">");
                    switch (item.screen)
                    {
                    case Screen::WorkMode:
                            lcd->setCursor(1,1);
                            lcd->print(String(WorkModeTxt[int(item.value)]));
                        break;
                        case Screen::Flag:
                            lcd->setCursor(lcdCols/2-1, 1);
                            lcd->print(item.value == 0 ? "NO" : "SI");
                        break;
                        case Screen::Number:
                            lcd->setCursor(lcdCols/2-1, 1);
                            lcd->print(item.value);
                            lcd->print(" ");
                        break;
                    
                    default:
                            lcd->setCursor(lcdCols/2-1, 1);
                            lcd->print("No SCREEN");
                        break;
                    }
                }

            }

            lcd->clear();
        }

        bool openMenu()
        {
            byte idxMenu       = 0;
            boolean exitMenu   = false;
            boolean forcePrint = true;


            while( !exitMenu )
            {
                btnPressed = readButtons();
                if( btnPressed == Button::Left && idxMenu-1 >= 0 )
                {
                    idxMenu--;
                    forcePrint = true;
                }
                else if( btnPressed == Button::Right && idxMenu+1 < this->menucompleto.getLength() )
                {
                    idxMenu++;
                    forcePrint = true;
                }
                else if( btnPressed == Button::Clicked )
                {
                    switch (int(this->menucompleto.get(idxMenu).screen))
                    {
                        case Screen::Number :
                        case Screen::Flag :
                        case Screen::WorkMode :
                        case Screen::Menu2 :
                            openSubMenuItem(this->menucompleto.get(idxMenu));
                            break;
                        case Screen::Exit :
                        default:
                            exitMenu = true;
                            break;
                    }
                    forcePrint = true;
                }


                if( !exitMenu && (forcePrint || btnPressed != Button::Unknown) )
                {
                    Serial.println("refresca pantalla");
                    this->lcd->clear();
                    forcePrint = false;

                    static const byte endFor1 = (this->menucompleto.getLength()+lcdRows-1)/lcdRows;
                    int graphMenu     = 0;

                    for( int i=1 ; i<=endFor1 ; i++ )
                    {
                        if( idxMenu < i*lcdRows )
                        {
                            graphMenu = (i-1) * lcdRows;
                            break;
                        }
                    }

                    byte endFor2 = graphMenu+lcdRows;

                    for( int i=graphMenu, j=0; i< endFor2 ; i++, j++ )
                    {
                        this->lcd->setCursor(1, j);
                        if(i<this->menucompleto.getLength()){
                            this->lcd->print(String(this->menucompleto.get(i).title));
                            Serial.println("titulo->"+String(i)+"->" + String(this->menucompleto.get(i).title));
                        }else{
                            lcd->print("                   ");
                            Serial.println("titulo vacio"+String(i));
                        }
                    }

                    for( int i=0 ; i<lcdRows ; i++ )
                    {
                        lcd->setCursor(0, i);
                        lcd->print(" ");
                    }
                    lcd->setCursor(0, idxMenu % lcdRows );
                    lcd->write(iARROW);
                }
            }

            lcd->clear();
            return true;
        }


        Button readButtons()
        {
            value += encoder->getValue();
            if (value != last) {
                Button state = (value<last) ? Button::Left : Button::Right;
                last = value;
                return state;
            }
            ClickEncoder::Button b = encoder->getButton();
            switch(b){
                case ClickEncoder::Pressed:
                    return Button::Pressed;
                break;
                case ClickEncoder::Held:
                    return Button::Held;
                break;
                case ClickEncoder::Released:
                    return Button::Released;
                break;
                case ClickEncoder::Clicked:
                    return Button::Clicked;
                break;
                case ClickEncoder::DoubleClicked:
                    return Button::DoubleClicked;
                break;
                default:
                    return Button::Unknown;
                break;
            }
            return Button::Unknown;
        }
};


#endif /* MENU_H */