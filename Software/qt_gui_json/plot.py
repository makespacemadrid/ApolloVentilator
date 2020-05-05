import pyqtgraph as pg
import sys
from random import randint
from PyQt5.QtCore import QTimer


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
