//#include <ESP8266WiFi.h>
//#include "HardwareSerial.h"
#include <ArduinoJson.h>
#include <EasyNextionLibrary.h>
//#include <SoftwareSerial.h>
#include "strings.h"

StaticJsonDocument<MAX_JSON_SIZE> json;



#define lcdBaudRate 115200
#define VentilatorSerial  Serial1
#define DisplaySerial     Serial2
#define DebugSerial       Serial


EasyNex        lcd(DisplaySerial);


#define PRESSURE_WAVE_ID 3
#define FLOW_WAVE_ID     25

void resetLCD(){
  lcd.writeStr("debug.txt", "Reset LCD\n");
  lcd.writeStr(""); 
  lcd.writeStr("peak.txt","0");    
  lcd.writeStr("flow.txt", "0"); 
  lcd.writeStr("cle "+String(PRESSURE_WAVE_ID)+",0");  
  lcd.writeStr("cle "+String(FLOW_WAVE_ID)+",0");

  lcd.writeStr("rpm.txt","0");
  lcd.writeStr("pMax.txt","0");
  lcd.writeStr("pPeak.txt","0");
  lcd.writeStr("pPeep.txt","0");
  lcd.writeStr("vTidal.txt","0");
  lcd.writeStr("ieRatio.txt","0");
  lcd.writeStr("iRiseTime.txt","0");
}


void sendVentilatorData(){
  
    String rpm     = lcd.readStr("rpm.txt");
    delay(10);
    String pMax    = lcd.readStr("pMax.txt");
    delay(10);
    String pPeak   = lcd.readStr("pPeak.txt");
    delay(10);
    String pPeep   = lcd.readStr("pPeep.txt");
    delay(10);
    String vTidal  = lcd.readStr("vTidal.txt");
    delay(10);
    String ieRatio = lcd.readStr("ieRatio.txt");
    delay(10);
    String iRiseT  = lcd.readStr("iRiseT.txt");

    JsonObject obj = json.to<JsonObject>();
   
    obj[STR_JSON_TYPE]             = STR_VENTILATOR_CONFIG;
    obj[STR_RESPIRATIONS_PER_MIN]  = rpm ;
    obj[STR_MAX_PRESSURE]          = pMax;
    obj[STR_PEAK_PRESSURE]         = pPeak;
    obj[STR_PEEP_PRESSURE]         = pPeep;
    obj[STR_TIDAL_VOLUME]          = vTidal;
    obj[STR_IE_RATIO]              = ieRatio;
    obj[STR_INSPIRATORY_RISE_TIME] = iRiseT ;

    serializeJson(obj, VentilatorSerial);
    VentilatorSerial.println();  
}

void trigger1(){
    DebugSerial.println("E->A: {type:'command',command:'getConfig'}");
    VentilatorSerial.println(F("{type:'command',command:'getConfig'}"));
    VentilatorSerial.flush();
    DebugSerial.println("E->A: {\"type\":\"command\",\"command\":\"start\"}");
    VentilatorSerial.println(F("{\"type\":\"command\",\"command\":\"start\"}"));
    VentilatorSerial.flush();
}

void trigger2(){  
    DebugSerial.println("E->A: {\"type\":\"command\",\"command\":\"stop\"}");
    VentilatorSerial.println(F("{\"type\":\"command\",\"command\":\"stop\"}"));
}

void trigger3(){  
    DebugSerial.println("E->A: {\"type\":\"command\",\"command\":\"test\"}");
    VentilatorSerial.println(F("{\"type\":\"command\",\"command\":\"test\"}"));
}

void trigger4(){
    DebugSerial.println("E->A: {\"type\":\"command\",\"command\":\"calibrate\"}");
    VentilatorSerial.println(F("{\"type\":\"command\",\"command\":\"calibrate\"}"));
}

void setup() {
  delay(1000);
  //Initialize serial and wait for port to open:
  VentilatorSerial.begin(lcdBaudRate, SERIAL_8N1, 33, 34, false, 2000); 
  VentilatorSerial.setRxBufferSize(1024);  
  VentilatorSerial.flush();
  DisplaySerial.begin(lcdBaudRate);
  DebugSerial.begin(lcdBaudRate);
  delay(500);           
  lcd.writeStr("page page0"); 
  delay(50);

  lcd.writeStr("debug.txt+", "Starting...\n");
  lcd.writeStr("rpm.txt","99");
  lcd.writeStr("pMax.txt","99");
  lcd.writeStr("pPeak.txt","99");
  lcd.writeStr("pPeep.txt","99");
  lcd.writeStr("vTidal.txt","99");
  lcd.writeStr("ieRatio.txt","99");
  lcd.writeStr("iRiseTime.txt","99");

  resetLCD();
  lcd.lastCurrentPageId = 1;

  VentilatorSerial.println("");
  VentilatorSerial.println(F("{type:'command',command:'getConfig'}"));
  VentilatorSerial.flush();                            
}

void readData(){
  
  if(VentilatorSerial.available() > 0){
    
      String payload = VentilatorSerial.readStringUntil('\n');      

     // DebugSerial.println("A->E: data: "+payload);
      
      DeserializationError error = deserializeJson(json, payload);
      
      if (error) { 
        DebugSerial.print("ERROR: ");
        DebugSerial.println(error.c_str());                
      }
      else
      {
  
          if(!json[STR_JSON_TYPE].isNull()){
              String type = json[STR_JSON_TYPE];
                            //mData
              //ventilatorStatus
              
              if(type == STR_VENTILATOR_CONFIG){
                 DebugSerial.println("vConfig received." );
                  delay(10);
                  String mode      = json[STR_VENTILATOR_MODE];
                  String stat      = json[STR_VENTILATOR_STATUS];
                  String rpm       = json[STR_RESPIRATIONS_PER_MIN];
                  String pMax      = json[STR_MAX_PRESSURE];
                  String pPeak     = json[STR_PEAK_PRESSURE];
                  String pPeep     = json[STR_PEEP_PRESSURE];                  
                  String vTrigger  = json[STR_TRIGGER_PRESSURE];
                  String vTidal    = json[STR_TIDAL_VOLUME];
                  String ieRatio   = json[STR_IE_RATIO];                
                  String iPause    = json[STR_INSPIRATORY_PAUSE];
                  // String iRiseTime = json[STR_INSPIRATORY_RISE_TIME];
                  
                  lcd.writeStr("rpm.txt",rpm);
                  lcd.writeStr("pMax.txt",pMax);
                  lcd.writeStr("pPeak.txt",pPeak);
                  lcd.writeStr("pPeep.txt",pPeep);
                  lcd.writeStr("vTidal.txt",vTidal);
                  lcd.writeStr("ieRatio.txt",ieRatio);
                  //lcd.writeStr("iRiseTime.txt",iRiseTime);

              }
              else if(type ==STR_MEDICAL_DATA){

                      int pressure  = json[STR_PRESSURE];
                      int flow      = json[STR_VOLUME];
                      
                      //Serial.println("peak: "+peak);
                  
                      //map(value, inputLow,inputHight,outputLow,outputHight)
                      //int point  = map(pressure, 0, 4025, 0, 100); 
                      int point  = map(pressure, -10, 50, 0, 255); 
                      int point2 = map(flow, 0, 1.5, 0, 255); 

                      String pointTxt  = String(point);
                      String pointTxt2 = String(point2);
                      
                      lcd.writeStr("peak.txt",String(pressure)); 
                      lcd.writeStr("add "+String(PRESSURE_WAVE_ID)+",0,"+pointTxt);
                       
                      lcd.writeStr("flow.txt", String(flow));
                      lcd.writeStr("add "+String(FLOW_WAVE_ID)+",0,"+pointTxt2);
                     
                  }
          }
      }
  }  
}



void loop() {/*
    if(Serial1.available() > 0){
    

      Serial.println("data: "+Serial1.read());
    }*/
    readData();
    lcd.NextionListen();


    // refereshCurrentPage();
  //  firstRefresh();
}
