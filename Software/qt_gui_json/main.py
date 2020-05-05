from PyQt5.QtGui import *
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import pyqtgraph as pg
from random import randint
from plot import Plot
from window import MainWindow
import sys

'''
Example data received
raw_data = b'{ "type" : "ventilatorConfig", "rpm" : 15,"pMax" : 50.0,"pPeak" : 45.0,"pPEEP" : 10,"vTidal" : 400,"ieRatio" : 0.33,"iRiseTime" : 1.0}
'''

def update_graph():
    pass

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
    timer.timeout.connect(update_graph)
    timer.start()

    window.show()

    sys.exit(app.exec_())