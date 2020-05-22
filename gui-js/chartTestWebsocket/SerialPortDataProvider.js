"use strict";

const SerialPort = require('serialport');
const Readline   = require('@serialport/parser-readline');

function SerialPortDataProvider(options, eventEmitter) {

    const serialPort = new SerialPort(
        options.serialportname, { 
            baudRate: options.serialportrate,
        }, function (err){
            if(err!=null){ eventEmitter.emit('serialPortError', err.message); }
        }
    );

    const parser = serialPort.pipe(new Readline({ delimiter: '\r\n' }));

    parser.on('data', function(payload){

        let payloadString = payload.toString();

        try {
            JSON.parse(payloadString);
            eventEmitter.emit('eventFromVentilator', payloadString);
        }
        catch(e){ eventEmitter.emit('debugMessage', payloadString ); }
    });

    serialPort.on('error', function(err) {
        eventEmitter.emit('serialPortError', err.message);
    })

    serialPort.on('close', function() {
        eventEmitter.emit('serialPortError','Serial port closed.');
    })

    eventEmitter.addListener('eventToVentilator', function(payload){
        try { serialPort.write(payload); }
        catch(e){}
    });
};

module.exports = SerialPortDataProvider;