from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import pyqtgraph as pg
from random import randint
import sys
import serial
import argparse
from itertools import count
import datetime as dt
import timeit


class CustomWidget(pg.PlotWidget):
    def __init__(self, index):
        super().__init__()
        self.index = index
        self.serial_port = serial.Serial(args.serialport, baudrate=115200, timeout=0)
        self.x = list(range(0,200))
        self.y = [randint(0,8) for _ in range(200)]
        self.setBackground('w')
        pen = pg.mkPen(color=(255, 0, 0))
        self.data_line =  self.plot(self.x, self.y, pen=pen)         
        self.timer = QTimer()
        self.timer.setInterval(5)
        self.timer.timeout.connect(self.update_plot_data)
        self.timer.start()

    def filter_data_from_serial_port(self):
        if self.serial_port.read() == b'D':
            if self.serial_port.in_waiting > 19:
                serial_line = b'D' + self.serial_port.read(19)
                serial_data = serial_line.decode('utf-8').split(':')
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
            self.y.append(float(data[1].split(',')[self.index]))
        else:
            self.y.append(0.0)  # Add a 0 value.

        self.data_line.setData(self.x, self.y)


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
        self.layout.addWidget(self.unpg)
        self.layout.addWidget(self.otropg)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--serialport', nargs='?', default='/dev/ttyUSB0', type=str, help='Serial port name')
    args = parser.parse_args()
    
    app = QApplication(sys.argv) 
    window = MainWindow()
    window.show()

    sys.exit(app.exec_())