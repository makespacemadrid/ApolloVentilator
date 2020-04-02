    
from PyQt5.QtChart import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from random import randint
from itertools import count


class ApolloWindow(QMainWindow):
    
    def __init__(self):
        super().__init__()
        self.x = count()
        self.pressure_series = QLineSeries()
        self.pressure_series.append(next(self.x),1)
        self.pressure_series.append(next(self.x),4)
        self.pressure_series.append(next(self.x),6)
        self.pressure_series.append(next(self.x),9)
        self.pressure_series.append(next(self.x),3)
        self.pressure_series.append(next(self.x),1)
        self.pressure_chart = QChart()
        self.pressure_chart.legend().hide()
        self.pressure_chart.addSeries(self.pressure_series)
        self.pressure_chart.createDefaultAxes()
        self.pressure_chart.setTitle('como salga voy a flipar')
        self.pressure_axis = QCategoryAxis()
        self.pressure_axis.append('1234', 1)
        self.pressure_axis.append('3453', 2)
        self.pressure_axis.append('1254', 3)
        self.pressure_axis.append('3557', 4)
        self.pressure_axis.append('1244', 5)
        self.pressure_chartview = QChartView(self.pressure_chart)
        self.pressure_chartview.setRenderHint(QPainter.Antialiasing)

        # self.volume_series = QLineSeries()
        # self.volume_series.append(0,1)
        # self.volume_series.append(1,4)
        # self.volume_series.append(2,6)
        # self.volume_series.append(3,9)
        # self.volume_series.append(4,3)
        # self.volume_series.append(5,1)
        # self.volume_chart = QChart()
        # self.volume_chart.legend().hide()
        # self.volume_chart.addSeries(self.volume_series)
        # self.volume_chart.createDefaultAxes()
        # self.volume_chart.setTitle('como salga voy a flipar dos veces')
        # self.volume_axis = QCategoryAxis()
        # self.volume_axis.append('1234', 2)
        # self.volume_axis.append('3453', 3)
        # self.volume_axis.append('1254', 4)
        # self.volume_axis.append('3557', 5)
        # self.volume_axis.append('1244', 6)
        # self.volume_chartview = QChartView(self.volume_chart)
        # self.volume_chartview.setRenderHint(QPainter.Antialiasing)


        # self.patata_series = QLineSeries()
        # self.patata_series.append(0,1)
        # self.patata_series.append(1,4)
        # self.patata_series.append(2,6)
        # self.patata_series.append(3,9)
        # self.patata_series.append(4,3)
        # self.patata_series.append(5,1)
        # self.patata_chart = QChart()
        # self.patata_chart.legend().hide()
        # self.patata_chart.addSeries(self.patata_series)
        # self.patata_chart.createDefaultAxes()
        # self.patata_chart.setTitle('como salga voy a flipar TRES veces')
        # self.patata_axis = QCategoryAxis()
        # self.patata_axis.append('1234', 2)
        # self.patata_axis.append('3453', 3)
        # self.patata_axis.append('1254', 4)
        # self.patata_axis.append('3557', 5)
        # self.patata_axis.append('1244', 6)
        # self.patata_chartview = QChartView(self.patata_chart)
        # self.patata_chartview.setRenderHint(QPainter.Antialiasing)

        self.widget = QWidget(self)
        self.setCentralWidget(self.widget)
        self.charts_layout = QGridLayout()
        self.charts_layout.addWidget(self.pressure_chartview)
        # self.charts_layout.addWidget(self.volume_chartview)
        # self.charts_layout.addWidget(self.patata_chartview)

        self.widget.setLayout(self.charts_layout)

        self.timer = QTimer()
        self.timer.setInterval(50)
        self.timer.timeout.connect(self.update_data)
        self.timer.start()

    def update_data(self):
        self.pressure_series.remove(0)
        self.pressure_series.append(next(self.x),randint(0,5))
        # self.pressure_axis.append('1244', 6)



a = QApplication([])

window = ApolloWindow()
window.resize(600, 300)
window.show()
a.exec_()