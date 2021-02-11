#include "Arduino.h"
#include <ArduinoJson.h>
#include "strings.h"
#include "waveforms.h"

#define oneHzSample 1000000/maxSamplesNum  // sample for the 1Hz signal expressed in microseconds
#define DisplaySerial Serial2

int i = 0;
int sample;


String payload;
StaticJsonDocument<300> json;

void setup() {
  DisplaySerial.begin(115200);
  Serial.begin(115200);
}

void log(){

    JsonObject obj = json.to<JsonObject>();

    float sin = waveformsTable[0][i]; 
    float tri = waveformsTable[1][i];
    float saw = waveformsTable[2][i];
    float squ = waveformsTable[3][i];

    i+=1;
    i = i%maxSamplesNum;
    
    obj[STR_JSON_TYPE]           = STR_MEDICAL_DATA;
    obj[STR_PRESSURE]            = (sin/500)-5;
    obj[STR_VOLUME]              = (sin/500)-5;
    obj[STR_INPUT_FLOW_INSTANT]  = (sin/500)-5;
    obj[STR_OUTPUT_FLOW_INSTANT] = (sin/500)-5;

    serializeJson(obj, DisplaySerial);
    DisplaySerial.println();
}

void loop() {
  
  if(Serial.available() > 0){
    
      payload = DisplaySerial.readStringUntil('\n');
      Serial.println("data: "+payload);
      
      DeserializationError error = deserializeJson(json, payload);
      
      if (error) { Serial.println("error"); }
      else{
  
          if(!json["type"].isNull()){
              String type = json["type"];
              if(type =="command"){
                  String command = json["command"];
                  if(command == "getConfig"){
                    
                      //Serial.println("getConfig");
                      
                      json[STR_JSON_TYPE]            = STR_VENTILATOR_CONFIG;
                      json[STR_RESPIRATIONS_PER_MIN] = 0 ;
                      json[STR_MAX_PRESSURE] =  1;
                      json[STR_PEAK_PRESSURE] = 2;
                      json[STR_PEEP_PRESSURE] = 3;
                      json[STR_TIDAL_VOLUME] = 4;
                      json[STR_IE_RATIO] = 5;
                      json[STR_INSPIRATORY_RISE_TIME] = 6 ;
                      serializeJson(json, DisplaySerial);
                      DisplaySerial.println();
                  }else if(command == "stop"){
                    
                      //Serial.println("getConfig");
                      
                      json[STR_JSON_TYPE]  = "vStatus";
                      json["hwStatus"] = "ok" ;
                      json["vStatus"] =  "stopped";
                      json["errorMSG"] = "input valve fail!";
                      serializeJson(json, Serial);
                      Serial.println();
                  }
              }
             
          }
      }
  
      payload="";   
  }

    
   log();
   //Serial.flush();
   
   delay(100);
/*
 * StaticJsonDocument<200> filter;
JsonObject& root = jsonBuffer.createObject();

  root["Moisture"] = data[0];
  root["Temperature"] = data[1];
  root["Humidity"] = data[2];

  Serial.println();
  root.prettyPrintTo(Serial);

  if(s.available()>0)
{
 root.printTo(s);
}

*/
}
