"use strict";

const mri        = require("mri");

const events     = require('events');
var eventEmitter = new events.EventEmitter();

const SerialPortDataProvider = require("./SerialPortDataProvider.js");
const SocketIODataProvider   = require("./SocketIODataProvider.js");
const MqttDataProvider       = require("./MqttDataProvider.js");
const Logger                 = require("./Logger.js");

const express    = require('express');
const app        = express();
const path       = require('path');
const server     = require('http').createServer(app);
const argv       = process.argv.slice(2);

const staticFilesFolder = 'client';

let options = mri( argv, { default: {
    serialportname: "/dev/ttyACM0",
    serialportrate: 921600,
    websocketport:  8081,
    mqtthost:      'localhost',
    mqttport:       1883,
    topic:          'ventilator/measurement/wilson',
    location:       'makeSpace',
    version:        '0.81'
}});

const serialPortProvider   = new SerialPortDataProvider(options, eventEmitter);
const socketIODataProvider = new SocketIODataProvider(server, eventEmitter);
const logger               = new Logger(options, eventEmitter);

if(options.mqtt!==undefined){
    const mqttProvider = new MqttDataProvider(options, eventEmitter);
}

eventEmitter.addListener('serialPortError', function(errorMessage){
    console.log(`Serial port error: ${errorMessage}`);
    process.exit(1);
});

eventEmitter.addListener('mqttError', function(errorMessage){
    console.log(`Mqtt error: ${errorMessage}`);
    process.exit(1);
});

server.listen(options.websocketport);

app.disable('x-powered-by');

app.get('/', function (req, res) {
    res.sendFile(__dirname +'/'+staticFilesFolder+ '/index.html');
});

app.use('/', express.static(path.join(__dirname, 'client')));