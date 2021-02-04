"use strict";

function SocketIODataProvider(server, eventEmitter, logger) {

    const io = require('socket.io')(server, { origins: '*:*'});

    const socketIOListener = function (receiveData){
        io.emit('serialPortData', { data: receiveData });
    }

    io.on('connection', function (socket) {
        socket.on('eventToVentilator', function (payload){
            let message = JSON.stringify(payload);
            eventEmitter.emit('eventToVentilator', message.toString());
        });
    });

    eventEmitter.addListener('eventFromVentilator', function(payload){
        io.emit('eventFromVentilator', { data: payload });
    });
};

module.exports = SocketIODataProvider;