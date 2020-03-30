const mri        = require("mri");

const SerialPort = require('serialport')
const Readline   = require('@serialport/parser-readline')

const app	 = require('express')();
const server = require('http').Server(app);
const io 	 = require('socket.io')(server, { origins: '*:*'});;

const argv   = process.argv.slice(2);

options = mri(argv, { default: {
	serialPortName: "/dev/ttyACM0",
 	serialPortRate: 115200,
	webSocketPort: 8081
}});

//serial port
const port   = new SerialPort(
	options.serialPortName, { 
		baudRate: options.serialPortRate,
		//parser: new SerialPort.parsers.Readline("\n")
	}, function (err) {
		if (err) {
    		return console.log('Serial port error: ', err.message);
  		}
	}
);

port.on('error', function(err) {
  console.log('Serial port rror: ', err.message)
})

port.on('close', function() {
	console.log('Port closed');
})

port.on('data', function (data) {
 	console.log('Data:', data.toString());
 	io.emit('serialPortData', { data: data.toString() });
});

//websocket server
server.listen(options.webSocketPort);

app.get('/', function (req, res) {
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function (socket) {
  	socket.on('newCommand', function (data) {
    	console.log(data);
  	});
});


