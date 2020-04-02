from PyQt5.QtChart import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from random import randint
from itertools import count


class ApolloWindow(QMainWindow):

    def __init__(self):
        super().__init__()
        self.serie = QLineSeries()
        self.chart = QChart()
        self.chart.addSeries(self.serie)
        self.chart.legend().hide()
        self.chart.createDefaultAxes()
        self.chart.setTitle('aversiladocutira')
        self.chartview = QChartView(self.chart) 
        self.chartview.setRenderHint(QPainter.Antialiasing)
        self.setCentralWidget(self.chartview)
        self.timer = QTimer()
        self.timer.setInterval(50)
        self.timer.timeout.connect(self.create_data)
        self.timer.start()

    def create_data(self):
        self.serie.append(randint(0,5),randint(0,8))

a = QApplication([])

w = ApolloWindow()
w.resize(600,300)
w.show()
a.exec_()