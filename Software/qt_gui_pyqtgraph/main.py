from PyQt5 import QtWidgets, uic, QtCore, QtGui
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import sys
import numpy as np
import functools

## Basis
# Plots with pyqtgraph use PlotWidget, which contains a QGraphics Scene (the surface rendered that manages canvas items).
# That includes title, labels, grids, axis ranges.
# QPen draws the line and manage all attributes related with it
# PlotWidget.clear

class AppDialog(QtWidgets.QDialog):
    def plot_graph(self, chart_index, graph, graph_title, hour):
        self.derivative[chart_index] = graph.plot(hour, title=graph_title)
        graph.setXRange(0, self.chunkSize, padding=0)
        # TODO: refactor
        if chart_index is 0:
            graph.setYRange(0, 50)
        else: 
            graph.setXRange(-20, 30)
        graph.disableAutoRange()
        graph.showGrid(True, True, 1)


    def __init__(self, *args, **kwargs):
        super(AppDialog, self).__init__(*args, **kwargs)
        self.derivative = [0,0,0]
        self.chunkSize = 200
        self.split = 100
        uic.loadUi('apollo_gui.ui', self)
        self.xAxis = np.arange(self.chunkSize)
        plot(0, self.graphPressure, "P", self.xAxis)
        plot(1, self.graphVolume, "V", self.xAxis)
        self.derivative[0].setPen(pg.mkPen('fbcca7', width=2))
        self.derivative[1].setPen(pg.mkPen('a3dade', width=2))


    def update_graph(self, pres, flow):

        self.derivative[0].setData(x=self.xAxis[:self.i+1], y=self.data1[:self.i+1,0])
        self.derivative[1].setData(x=self.xAxis[:self.i+1], y=self.data1[:self.i+1,1])
        

def main():
    app = QtWidgets.QApplication(sys.argv)
    app_dialog = AppDialog()
    app_dialog.show()
    timer = pg.QtCore.QTimer()
    wrappedPlotUpdate= functools.partial(update_graph, main) #TODO: Why
    timer.timeout.connect(update_graph)
    timer.start(50)
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
    

