from PyQt5 import QtCore, QtGui, QtWidgets
from threading import Thread
from collections import deque
import pyqtgraph as pg
import numpy as np
import random
import sys
import time

"""Scrolling Timestamp Plot Widget Example"""

class TimeAxisItem(pg.AxisItem):
    """Internal timestamp for x-axis"""
    def __init__(self, *args, **kwargs):
        super(TimeAxisItem, self).__init__(*args, **kwargs)

    def tickStrings(self, values, scale, spacing):
        """Function overloading the weak default version to provide timestamp"""

        return [QtCore.QTime().currentTime().addMSecs(value).toString('mm:ss') for value in values]

class ScrollingTimestampPlot(QtGui.QWidget):
    """Scrolling plot widget with timestamp on x-axis and dynamic y-axis"""

    def __init__(self, parent=None):
        super(ScrollingTimestampPlot, self).__init__(parent)

        # Internal timestamp for x-axis
        self.timestamp = QtCore.QTime()
        self.timestamp.start()

        # Desired Frequency (Hz) = 1 / self.FREQUENCY
        # USE FOR TIME.SLEEP (s)
        self.FREQUENCY = 0.025

        # Screen refresh rate to update plot (ms)
        # self.SCROLLING_TIMESTAMP_PLOT_REFRESH_RATE = 1 / Desired Frequency (Hz) * 1000
        # USE FOR TIMER.TIMER (ms)
        self.SCROLLING_TIMESTAMP_PLOT_REFRESH_RATE = self.FREQUENCY * 1000

        self.DATA_POINTS_TO_DISPLAY = 200

        # Automatically pops from left if length is full
        self.data = deque(maxlen=self.DATA_POINTS_TO_DISPLAY)

        # Create Plot Widget 
        self.scrolling_timestamp_plot_widget = pg.PlotWidget(axisItems={'bottom': TimeAxisItem(orientation='bottom')})

        # Enable/disable plot squeeze (Fixed axis movement)
        self.scrolling_timestamp_plot_widget.plotItem.setMouseEnabled(x=False, y=False)
        self.scrolling_timestamp_plot_widget.setTitle('Scrolling Timestamp Plot Example')
        self.scrolling_timestamp_plot_widget.setLabel('left', 'Value')
        self.scrolling_timestamp_plot_widget.setLabel('bottom', 'Time (s)')

        self.scrolling_timestamp_plot = self.scrolling_timestamp_plot_widget.plot()
        self.scrolling_timestamp_plot.setPen(246,212,255)

        self.layout = QtGui.QGridLayout()
        self.layout.addWidget(self.scrolling_timestamp_plot_widget)

        self.read_position_thread()
        self.start()

    def start(self):
        """Update plot"""

        self.position_update_timer = QtCore.QTimer()
        self.position_update_timer.timeout.connect(self.plot_updater)
        self.position_update_timer.start(self.get_scrolling_timestamp_plot_refresh_rate())

    def read_position_thread(self):
        """Read in data using a thread"""

        self.current_position_value = 0
        self.position_update_thread = Thread(target=self.read_position, args=())
        self.position_update_thread.daemon = True
        self.position_update_thread.start()

    def read_position(self):
        frequency = self.get_scrolling_timestamp_plot_frequency()
        while True:
            self.current_position_value = random.randint(1,101) 
            time.sleep(frequency)

    def plot_updater(self):
        self.data_point = float(self.current_position_value)

        self.data.append({'x': self.timestamp.elapsed(), 'y': self.data_point})
        self.scrolling_timestamp_plot.setData(x=[item['x'] for item in self.data], y=[item['y'] for item in self.data])

    def clear_scrolling_timestamp_plot(self):
        self.data.clear()

    def get_scrolling_timestamp_plot_frequency(self):
        return self.FREQUENCY

    def get_scrolling_timestamp_plot_refresh_rate(self):
        return self.SCROLLING_TIMESTAMP_PLOT_REFRESH_RATE

    def get_scrolling_timestamp_plot_layout(self):
        return self.layout

    def get_current_position_value(self):
        return self.current_position_value

    def get_scrolling_timestamp_plot_widget(self):
        return self.scrolling_timestamp_plot_widget

# Start Qt event loop unless running in interactive mode or using pyside
if __name__ == '__main__':
    # Create main application window
    app = QtWidgets.QApplication([])
    app.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))
    mw = QtGui.QMainWindow()
    mw.setWindowTitle('Scrolling Plot Example')

    # Create scrolling plot
    scrolling_timestamp_plot_widget = ScrollingTimestampPlot()

    # Create and set widget layout
    # Main widget container
    cw = QtGui.QWidget()
    ml = QtGui.QGridLayout()
    cw.setLayout(ml)
    mw.setCentralWidget(cw)

    # Can use either to add plot to main layout
    #ml.addWidget(scrolling_timestamp_plot_widget.get_scrolling_timestamp_plot_widget(),0,0)
    ml.addLayout(scrolling_timestamp_plot_widget.get_scrolling_timestamp_plot_layout(),0,0)
    mw.show()

    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()