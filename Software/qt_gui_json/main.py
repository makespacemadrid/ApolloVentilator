from PyQt5.QtGui import *
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import pyqtgraph as pg
from random import random, randint
# from plot import Plot
from window import MainWindow
import sys
import functools


'''
Example data received
raw_data = b'{ "type" : "ventilatorConfig", "rpm" : 15,"pMax" : 50.0,"pPeak" : 45.0,"pPEEP" : 10,"vTidal" : 400,"ieRatio" : 0.33,"iRiseTime" : 1.0}
'''
#TODO: change from bytestring to string

def update_graph():
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

class Plot(pg.PlotWidget):
    def __init__(self, index):
        super().__init__()
        self.index = index
        
        self.x = list(range(0,200))
        self.y = [randint(0,8) for _ in range(200)]
        self.setBackground('w')
        pen = pg.mkPen(color=(255, 0, 0))
        self.data_line =  self.plot(self.x, self.y, pen=pen)         
        
    def plot_graph(self):
        self.x = list(range(0,200))
        self.y = [randint(0,8) for _ in range(200)]
        print(self.x, self.y)
        self.data_line.setData(self.x, self.y)


    def update_plot_data(self, data): 
        self.x = self.x[1:]  # Remove the first y element.
        self.x.append(self.x[-1] + 1)   # Add a new value 1 higher than the last.

        self.y = self.y[1:]  # Remove the first 
        # if len(selfdata) > 0:
        try:
            self.y.append(data)
            # self.data_line.setData(self.x, self.y)
        except IndexError:
            print('error getting data ')
        except ValueError:
            print('error converting data ')
        except:
            print('other kind of error ', data, sys.exc_info()[0])


def plot_graphs(plot1,plot2,plot3,plot4):
    print('plot_graphs')
    plot1.plot_graph()
    plot2.plot_graph()
    plot3.plot_graph()
    plot4.plot_graph()


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
    timer.setInterval(10)
    plot_process = functools.partial(plot_graphs,plot1=unpg, plot2=otropg, plot3=yotro,plot4=otrouanmortaim)
    timer.timeout.connect(plot_process)
    timer.start()

    window.show()

    sys.exit(app.exec_())