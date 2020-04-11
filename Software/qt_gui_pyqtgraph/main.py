from PyQt5.QtGui import *
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import pyqtgraph as pg
from random import randint
import io
import sys
import glob
import serial
import argparse
from itertools import count
import datetime as dt
import timeit
import socketio
from classes.window import MainWindow
from classes.utils import serial_ports_get
from classes.plot import CustomWidget
import functools


def process_data(plot1, plot2, plot3, plot4):
    try:
        debug = wrapper_text.readline()
        print(debug, type(debug))
        data = debug.split(':')
        print(len(data), data)
        print(wrapper_text.readline(), type(wrapper_text.readline()))
        print(data[1].split(',')[0])
    except:
        print(debug)

    # plot1.update_plot_data(data[1].split(',')[0])
    # plot2.update_plot_data(data[1].split(',')[1])
    # plot3.update_plot_data(data[1].split(',')[2])
    # plot4.update_plot_data(data[1].split(',')[3])


def plot_graphs():
    plot1.plot_graph()
    plot2.plot_graph()
    plot3.plot_graph()
    plot4.plot_graph()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--serialport', nargs='?', default='/dev/ttyUSB0', type=str, help='Serial port name')
    parser.add_argument('--socket', action='store_true', help='Socket io name')
    parser.add_argument('--auto', action='store_true', help='Automatically select serial port')

    args = parser.parse_args()

    if args.auto:
        serial_ports = serial_ports_get()
        print('Detected > ', serial_ports, ' Choosing first one')
        try:
            serial_port = serial.Serial(serial_ports[0], baudrate=115200, timeout=0)
        except:
            print('ERROR: Serial port not connected')
            sys.exit(1)
    elif args.socket:
        s = socketio.Client()
        try:
            s.connect('http://makespacemadrid.synology.me:9999')
        except ConnectionError:
            print('Could not connect to mks. Ask Javi')
            sys.exit(1)
        except:
            print('Another kind of error')
            sys.exit(1)
    else:
        serial_port = serial.Serial(args.serialport, baudrate=115200, timeout=0)
    buffer_size = io.DEFAULT_BUFFER_SIZE # https://docs.python.org/3/library/io.html#io.DEFAULT_BUFFER_SIZE
    buffer = io.BufferedReader(serial_port, buffer_size)
    
    app = QApplication(sys.argv) 
    window = MainWindow()   

    unpg = CustomWidget(0)
    otropg = CustomWidget(1)
    yotro = CustomWidget(2)
    otrouanmortaim = CustomWidget(3)

    window.layout.addWidget(unpg)
    window.layout.addWidget(otropg)
    window.layout.addWidget(yotro)
    window.layout.addWidget(otrouanmortaim)

    global wrapper_text 
    wrapper_text = io.TextIOWrapper(buffer)

    timer = QTimer()
    timer.setInterval(10)
    update_process = functools.partial(process_data, plot1=unpg, plot2=otropg, plot3=yotro,plot4=otrouanmortaim)
    timer.timeout.connect(update_process)
    timer.start()

    timer_plot = QTimer()
    timer_plot.setInterval(2)
    timer_plot.timeout.connect(plot_graphs)
    timer_plot.start()


    
    window.show()

    sys.exit(app.exec_())