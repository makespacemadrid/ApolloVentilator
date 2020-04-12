# Install dependencies
npm install

---
# Websocket server
Params:
--serialPortName="serialPortName"     default: /dev/ttyACM0
--serialPortRate="serialPortBaudRate" default: 115200
--webSocketPort="webSocketPort>       default: 8081
--verbose

node webscoket.js --serialPortName="/dev/tty.usbserial-arduino"  --serialPortRate="115200" --webSocketPort="8081" --verbose

Showcase :

Main view      http://localhost:8081/
Full view      http://localhost:8081/full
Log view       http://localhost:8081/log
Log view WebGL http://localhost:8081/logwebl

---

# Mqtt client
Params:
--serialPortName="serialPortName"     default: /dev/ttyACM0
--serialPortRate="serialPortBaudRate" default: 115200
--host="mqttServer"                   default: localhost
--port="mqttPort"                     default: 1883
--verbose 

node mqtt.js   --serialPortName="/dev/tty.usbserial-arduino" --host="host" --port="63636" --topic="ventilator/measurement/wilson" --verbose
