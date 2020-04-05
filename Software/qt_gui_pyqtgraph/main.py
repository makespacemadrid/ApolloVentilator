from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import pyqtgraph as pg
from random import randint
import sys
import glob
import serial
import argparse
from itertools import count
import datetime as dt
import timeit


class CustomWidget(pg.PlotWidget):
    def __init__(self, index):
        super().__init__()
        self.index = index
        # import socket
        # s = socket.socket()
        # s.connect(('90.170.17.126', 9999))
        if args.auto:
            serial_ports = self.serial_ports_get()
            print('Detected > ', serial_ports, ' Choosing first one')
            self.serial_port = serial.Serial(serial_ports[0], baudrate=115200, timeout=0)
        else:
            self.serial_port = serial.Serial(args.serialport, baudrate=115200, timeout=0)
        self.x = list(range(0,200))
        self.y = [randint(0,8) for _ in range(200)]
        self.setBackground('w')
        pen = pg.mkPen(color=(255, 0, 0))
        self.data_line =  self.plot(self.x, self.y, pen=pen)         
        self.timer = QTimer()
        self.timer.setInterval(2)
        self.timer.timeout.connect(self.update_plot_data)
        self.timer.start()

    def serial_ports_get(self):
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
            # if self.serial_port.in_waiting > 41:
            #     serial_line = b'D' + self.serial_port.read(19)
            serial_data = str(serial_line).split(':')
            print(serial_data)
            return serial_data
        else:
            return []

    def update_plot_data(self):
        data = self.filter_data_from_serial_port()
        self.x = self.x[1:]  # Remove the first y element.
        self.x.append(self.x[-1] + 1)   # Add a new value 1 higher than the last.

        self.y = self.y[1:]  # Remove the first 
        if len(data) > 0:
            try:
                self.y.append(float(data[1].split(',')[self.index]))
            except IndexError:
                print('error getting data ', self.index)
            except ValueError:
                print('error converting data ', self.index)
            except:
                print('other kind of error ', data)

        else:
            self.y.append(0.0)  # Add a 0 value.
        try:
            self.data_line.setData(self.x, self.y)
        except:
            print('error setting data ', self.x, self.y)


class MainWindow(QWidget):
    
    def __init__(self):
        super().__init__()

        self.title = 'Apollo Ventilator App'
        self.left = 100
        self.top = 100
        self.width = 720
        self.height = 500
        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top,self.width,self.height)
        self.layout = QGridLayout(self)
        self.unpg = CustomWidget(0)
        self.otropg = CustomWidget(1)
        self.yotro = CustomWidget(2)
        self.otrouanmortaim = CustomWidget(3)
        self.layout.addWidget(self.unpg)
        self.layout.addWidget(self.otropg)
        self.layout.addWidget(self.yotro)
        self.layout.addWidget(self.otrouanmortaim)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--serialport', nargs='?', default='/dev/ttyUSB0', type=str, help='Serial port name')
    parser.add_argument('--socket', nargs='?', type=str, help='Socket name')
    parser.add_argument('--auto', action='store_true', help='Automatically select serial port')

    args = parser.parse_args()
    
    app = QApplication(sys.argv) 
    window = MainWindow()
    window.show()

    sys.exit(app.exec_())