"use strict";

function MqttDataProvider(options, eventEmitter) {

    const mqtt = require('mqtt')

    let mqttOptions = { port: options.mqttport, protocolVersion:4 }
    let client      = mqtt.connect('mqtt://'+options.mqtthost, mqttOptions)

    let pacient  = options.topic.split('/').pop();
    let location = options.location;
    let version  = options.version;
    let topic    = options.topic;

    client.on('connect', function() {
    
        //eventEmitter.emit('infoLog', 'Mqtt client connected');
        
        eventEmitter.addListener('eventFromVentilator', function(payload){

            let ventilatorData  = JSON.parse(payload);
            let messageType     = ventilatorData.type;

            ventilatorData.time = Date.now()+'000000';
                          
            let jsonPayload = [
                ventilatorData,
                { 
                    tag1: pacient ,
                    tag2: location,
                    tag3: version
                }
            ];
    
            let mqttPayload = JSON.stringify(jsonPayload);
            let mqttTopic   = topic+'/'+messageType;

            client.publish(mqttTopic, mqttPayload);
            
        });
     
    });

    client.on('error', function(err) {
        eventEmitter.emit('mqttError',err.message);
    });

};

module.exports = MqttDataProvider;