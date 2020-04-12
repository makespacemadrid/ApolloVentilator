"use strict";
const mri        = require("mri");
const SerialPort = require('serialport')
const Readline   = require('@serialport/parser-readline')

const express    = require('express');
const app        = express();
const path       = require('path');
const server     = require('http').createServer(app);
const io         = require('socket.io')(server, { origins: '*:*'});;
const argv       = process.argv.slice(2);

const staticFilesFolder = 'client';

let options = mri(argv, { default: {
    serialPortName: "/dev/ttyACM0",
    serialPortRate: 115200,
    webSocketPort:  8081
}});

const noLog    = function(){};
const showLog  = function(log){ console.log(log); };
const printLog = options.verbose!==undefined?showLog:noLog;

//serial port
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

const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

parser.on('data',function(data){
    let receiveData = data.toString();
    printLog("Serial port data: "+ receiveData);
    io.emit('serialPortData', { data: receiveData });
});

port.on('error', function(err) {
    showLog('Serial port rror: '+ err.message);
    process.exit(1);
})

port.on('close', function() {
    showLog('Serial port rror: '+ err.message);
    process.exit(0);
})

//websocket server

server.listen(options.webSocketPort);

app.disable('x-powered-by');

app.get('/', function (req, res) {
    res.sendFile(__dirname +'/'+staticFilesFolder+ '/index.html');
});

app.get('/log', function (req, res) {
    res.sendFile(__dirname +'/'+staticFilesFolder+ '/log.html');
});

app.get('/logwebgl', function (req, res) {
    res.sendFile(__dirname +'/'+staticFilesFolder+ '/logWebGL.html');
});

app.get('/full', function (req, res) {
    res.sendFile(__dirname +'/'+staticFilesFolder+ '/full.html');
});


app.use('/', express.static(path.join(__dirname, 'client')));

io.on('connection', function (socket) {
    socket.on('newCommand', function (data) {
        printLog('Socket newCommand data: '+ data.toString());
    });
});