import serial
import socketio
import sys
import glob


def serial_ports_get():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')
    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def filter_data_from_serial_port(self):
    if self.serial_port.read() == b'D':
        serial_line = b'D' + self.serial_port.readline()
        serial_data = str(serial_line).split(':')
        print(serial_data)
        return serial_data
    else:
        return []


def filter_data_from_socketio():
    serial_data = []
    @s.on('serialPortData') 
    def on_message(data):
        if 'DATA' in data:
            serial_line = data.split("'")[3]
            serial_data = str(serial_line).split(':')
            print(data)
    return serial_data
