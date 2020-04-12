"use strict";
const mri         = require("mri");
const SerialPort  = require('serialport')
const Readline	  = require('@serialport/parser-readline')
const mqtt        = require('mqtt')
const argv        = process.argv.slice(2);

let options = mri( argv, { default: {
    serialPortName: "/dev/ttyACM0",
    serialPortRate: 115200,
    host:           'localhost',
    port:           1883,
    topic:          'ventilator/measurement/wilson',
}});

const noLog    = function(){};
const showLog  = function(log){ console.log(log); };
const printLog = options.verbose!==undefined?showLog:noLog;

const port = new SerialPort(
    options.serialPortName, { 
        baudRate: options.serialPortRate,
    }, function (err) {
        if (err) {
            showLog('Serial port: '+ err.message);
            process.exit(1);
        }
    }
);

const parser    = port.pipe(new Readline({ delimiter: '\r\n' }));
let mqttOptions = { port: options.port }
var client      = mqtt.connect('mqtt://'+options.host, mqttOptions)

showLog("Connecting...");

client.on('connect', function() {

    showLog('Connected');

    parser.on('data',function(data){
	
        let id          = options.topic.split('/').pop();
        let receiveData = data.toString();
        let rawCommand  = receiveData.split(':');
        let commandID   = rawCommand[0];
    
        printLog("Serial port data: "+ receiveData);
    
        if (commandID === 'DATA'){
    
            let ventilatorData	   = rawCommand[1].split(',');
            
            let pressure           = parseFloat(ventilatorData[0]);
            let intakeInstantFlow  = parseFloat(ventilatorData[1]);
            let exitInstantFlow    = parseFloat(ventilatorData[2]);
            let intakeFlow         = parseFloat(ventilatorData[3]);
            let exitFlow           = parseFloat(ventilatorData[4]);
            let intakeValve        = parseFloat(ventilatorData[5]);
            let exitValve          = parseFloat(ventilatorData[6]);
            let status             = parseFloat(ventilatorData[7]);
    
            let payload = [{
                    pressure          : pressure,
                    intakeInstantFlow : intakeInstantFlow,
                    exitInstantFlow   : exitInstantFlow,
                    intakeFlow        : intakeFlow,
                    intakeValve       : intakeValve,
                    exitValve         : exitValve,
                    status            : status
                },{ 
                    tag1: id 
                }
            ];
            client.publish(options.topic, JSON.stringify(payload));
        }
    });
    
    port.on('error', function(err) {
        showLog('Serial port error: '+ err.message);
        process.exit(1);
    })
    
    port.on('close', function() {
        showLog('Port closed');
    })
});

client.on('error', function(err) {
    showLog('Mqtt error: '+ err,message);
    process.exit(1);
});