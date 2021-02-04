# Install dependencies
npm install

---
#Ventilator test app
##Params:

###Serial port:

- --serialportname="serialPortName"     default: /dev/ttyACM0
- --serialportrate=<serialPortBaudRate> default: 921600

###Websocket:

- --websocketport=<webSocketPort>       default: 8081

###MQTT:

- --mqtt                                "Enable mqtt"
- --mqtthost="mqttServer"               default: localhost
- --mqttport=<mqttPort>                 default: 1883
- --topic="mqttMainTopic"               default: "ventilator/measurement/wilson/#"
- --location="ventilatorLocationName"   default:"makeSpace"
- --version="firmwareID"                default:"0.81"

###Log

- --inputlog                            "Show messages to ventilator"
- --outputlog                           "Show messages from ventilator"
- --debuglog                            "Show ventilator debug messages"


node gui.js --serialportname="/dev/tty.usbserial-arduino" --serialportrate=115200 --mqtt --mqtthost="mqtt.me" --mqttport=1234 --topic="ventilator/measurement/test" --inputlog --outputlog --debuglog

###Showcase :

Main view      http://localhost:8081/

---

#Mqtt sub client
##Params:

--mqtthost="mqttServer"                   default: localhost

--port="mqttport"                     default: 1883

--topic="mqttMainTopic"               default: "ventilator/measurement/wilson/#"


node mqttSubscribe.js  --host="mqtt.me" --port=1234 --topic="ventilator/measurement/wilson/#"