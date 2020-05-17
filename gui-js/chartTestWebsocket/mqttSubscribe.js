"use strict";
const mri         = require("mri");
const SerialPort  = require('serialport')
const Readline	  = require('@serialport/parser-readline')
const mqtt        = require('mqtt')
const argv        = process.argv.slice(2);

let options = mri( argv, { default: {
    serialPortName: "/dev/ttyACM0",
    serialPortRate: 921600,
    host:           'localhost',
    port:           1883,
    topic:          'ventilator/measurement/wilson',
    location:       'makeSpace',
    version:        '0.81'
}});

const showLog    = function(log){ console.log(log); };

let mqttOptions = { port: options.port }
var client      = mqtt.connect('mqtt://'+options.host, mqttOptions)

showLog("Connecting...");

client.on('connect', function() {

    showLog('Mqtt client connected');

    client.subscribe(options.topic, function (err) {
        if (err) {
            showLog('Mqtt client not connected');
        }
    });

});

client.on('message', function (topic, message) {
    showLog("Mqtt message: "+ message.toString()+'\n');
});

client.on('error', function(err) {
    showLog('Mqtt error: '+ err,message);
    process.exit(1);
});