from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import pyqtgraph as pg
from random import randint
import sys
import serial
import argparse


class MainWindow(QMainWindow):
    
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
        self.graphWidget = pg.PlotWidget()
        self.setCentralWidget(self.graphWidget)
        self.serial_port = serial.Serial(args.serialport, baudrate=115200, timeout=0)
        self.x = list(range(100))  # 100 time points
        self.y = [randint(0,100) for _ in range(100)]  # 100 data points

        self.graphWidget.setBackground('w')

        pen = pg.mkPen(color=(255, 0, 0))
        self.data_line =  self.graphWidget.plot(self.x, self.y, pen=pen)
        
        self.timer = QTimer()
        self.timer.setInterval(50)
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
        self.x.append(self.x[-1] + 1)  # Add a new value 1 higher than the last.

        self.y = self.y[1:]  # Remove the first 
        if len(data) > 0:
            self.y.append(float(data[1].split(',')[0]))
        else:
            self.y.append(0.0)  # Add a new random value.

        self.data_line.setData(self.x, self.y)


        # self.textbox = QLineEdit(self)
        # self.textbox.move(20,20)
        # self.textbox.resize(280,40)

        # self.button = QPushButton('Connect', self)
        # self.button.move(20,80)

        # self.button.clicked.connect(self.on_click)
        # layout = QVBoxLayout()
        # widget = QWidget()
        # widget.setLayout(layout)
        self.show()

    @pyqtSlot()
    def on_click(self):
        textboxValue = self.textbox.text()
        QMessageBox.question(self, 'Confirm connection', "Connected to: " + textboxValue, QMessageBox.Ok, QMessageBox.Ok)
        self.textbox.setText("")
  


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--serialport', nargs='?', default='/dev/ttyACM0', type=str, help='Serial port name')
    args = parser.parse_args()
    
    app = QApplication(sys.argv) 
    window = MainWindow()

    sys.exit(app.exec_())