from PyQt5.QtGui import *
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import pyqtgraph as pg
from random import random, randint
# from plot import Plot
from window import MainWindow
import sys
import functools
from itertools import count


'''
Example data received
raw_data = b'{ "type" : "ventilatorConfig", "rpm" : 15,"pMax" : 50.0,"pPeak" : 45.0,"pPEEP" : 10,"vTidal" : 400,"ieRatio" : 0.33,"iRiseTime" : 1.0}
'''
#TODO: change from bytestring to string

class Plot(pg.PlotWidget):
    def __init__(self, index):
        super().__init__()
        self.index = index
        self.pseudotime = count()
        self.buffer = 100
        self.x = [next(self.pseudotime)]
        self.y = [randint(0,8)]
        print("Initialized ", self.x, self.y)
        self.setBackground('w')
        pen = pg.mkPen(color=(255, 0, 0))
        self.data_line =  self.plot(self.x, self.y, pen=pen)         
        
    def plot_graph(self, data):
        self.x.append(next(self.pseudotime))
        self.y.append(data)
        print("Updated ", self.x, self.y)
        if len(self.x) > self.buffer:
            # print('buffer length achieved')
            self.x = self.x[1:]
            self.y = self.y[1:]
        self.data_line.setData(self.x, self.y)


def plot_graphs(plot1,plot2,plot3,plot4):
    raw_data = { 
        "type" : "ventilatorConfig", 
        "rpm" : random(),
        "pMax" : random(),
        "pPeak" : random(),
        "pPEEP" : random(),
        "vTidal" : random(),
        "ieRatio" : random(),
        "iRiseTime" : random()
    }
    print('plot_graphs')

    plot1.plot_graph(raw_data['pMax'])
    plot2.plot_graph(raw_data['pPeak'])
    plot3.plot_graph(raw_data['pPEEP'])
    plot4.plot_graph(raw_data['rpm'])


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()   

    unpg = Plot(0)
    otropg = Plot(1)
    yotro = Plot(2)
    otrouanmortaim = Plot(3)

    window.layout.addWidget(unpg)
    window.layout.addWidget(otropg)
    window.layout.addWidget(yotro)
    window.layout.addWidget(otrouanmortaim)
    timer = QTimer()
    timer.setInterval(100)
    plot_process = functools.partial(plot_graphs,plot1=unpg, plot2=otropg, plot3=yotro,plot4=otrouanmortaim)
    timer.timeout.connect(plot_process)
    timer.start()

    window.show()

    sys.exit(app.exec_())