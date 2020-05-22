"use strict";

const mri         = require("mri");
const mqtt        = require('mqtt')
const argv        = process.argv.slice(2);

let options = mri( argv, { default: {
    mqtthost:       'localhost',
    mqttport:        1883,
    topic:          'ventilator/measurement/wilson/#',
}});

let mqttOptions = { port: options.mqttport }
var client      = mqtt.connect('mqtt://'+options.mqtthost, mqttOptions)

console.log("Connecting...");

client.on('connect', function() {

    console.log('Mqtt client connected');

    client.subscribe(options.topic, function (err) {
        if (err) {
            console.log('Mqtt client not connected');
        }
    });

});

client.on('message', function (topic, message) {
    console.log("Mqtt message: "+ message.toString()+'\n');
});

client.on('error', function(err) {
    console.log('Mqtt error: '+ err,message);
    process.exit(1);
});