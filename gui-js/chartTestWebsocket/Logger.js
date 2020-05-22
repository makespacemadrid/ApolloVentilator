"use strict";

function Logger(options, eventEmitter) {

    if(options.inputlog!==undefined){
        eventEmitter.addListener('eventToVentilator', function(payload){
            console.log(`DataToVentilator: ${payload}`);
        });
    }
    
    if(options.outputlog!==undefined){
        eventEmitter.addListener('eventFromVentilator', function(payload){
            console.log(`DataFromVentilator: ${payload.toString()}`);
        });
    }
    
    if(options.debuglog!==undefined){
        eventEmitter.addListener('debugMessage', function(payload){
            console.log(`Debug message: ${payload}`);
        });
    }

    eventEmitter.addListener('infoLog', function(payload){
        console.log(`Info message: ${payload}`);
    });    

};

module.exports = Logger;