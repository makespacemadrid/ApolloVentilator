# Install dependencies
npm install

---
# Websocket server
Params:

--serialPortName="serialPortName"     default: /dev/ttyACM0

--serialPortRate="serialPortBaudRate" default: 115200

--webSocketPort="webSocketPort>       default: 8081

--verbose                             "Show log messages"


node webscoket.js --serialPortName="/dev/tty.usbserial-arduino"  --serialPortRate="115200" --webSocketPort="8081" --verbose

Showcase :

Main view      http://localhost:8081/

Full view      http://localhost:8081/full

Log view       http://localhost:8081/log

Log view WebGL http://localhost:8081/logwebl

---

# Mqtt pub client
Params:

--serialPortName="serialPortName"     default: /dev/ttyACM0

--serialPortRate="serialPortBaudRate" default: 115200

--host="mqttHost"                     default: localhost

--port="mqttPort"                     default: 1883

--topic="mqttMainTopic"               default: "ventilator/measurement/wilson"

--location="location"                 default: "Makespace"

--version="firmwareVersion"           default: 0.81

--verbose                             "Show JSON log messages"

--debug                               "Show hardware debug messages"


node mqtt.js   --serialPortName="/dev/tty.usbserial-arduino" --host="mqttHost" --port="63636" --topic="ventilator/measurement/wilson" --verbose

---

# Mqtt sub client
Params:

--host="mqttServer"                   default: localhost

--port="mqttPort"                     default: 1883

--topic="mqttMainTopic"               default: "ventilator/measurement/wilson"


node mqttSubscribe.js  --host="mqttHost" --port="63636" --topic="ventilator/measurement/wilson"