#include "ApolloConnect.h";

void ApolloConnect::send(){
    String str = String(this->_rpm)+","+
    String(this->_mlTidalVolume)+","+
    String(this->_porcentajeInspiratorio)+","+
    String(this->_pressionPeep)+","+
    String(this->_sexo)+","+
    String(this->_weight)+","+
    String(this->_height)+","+
    String(this->_pressionMax)+","+
    String(this->_pressionMeseta)+","+
    String(this->_pressionPico)+","+
    String(this->_porcentajeFiO2)+","+
    String(this->_presionTriggerInspiration)+","+
    String(this->_mode);
    Serial.println(str);
}