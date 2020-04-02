#!/usr/bin/env python

from PyQt5 import QtWidgets, uic

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt

import sys


class GUI(QtWidgets.QMainWindow):
    __doc__ = \
        """
        From:
        - https://nitratine.net/blog/post/how-to-import-a-pyqt5-ui-file-in-a-python-gui/
        - https://stackoverflow.com/questions/12459811/how-to-embed-matplotlib-in-pyqt-for-dummies
        """

    def __init__(self):
        super(GUI, self).__init__()
        uic.loadUi('app.ui', self)
        # Configure matplotlib figure > attach to FigureCanvas > attach to NavigationToolbar
        self.matplot_figure = plt.Figure()
        self.matplot_canvas = FigureCanvas(self.matplot_figure)

        self.qt_plot_widget = self.findChild(QtWidgets.QWidget, 'plot_widget')

        self.show()


class App(object):
    __doc__ = """"""

    def __init__(self):
        self.qt_app = QtWidgets.QApplication(sys.argv)
        self.ui = GUI()

    def run(self):
        self.qt_app.exec_()


if __name__ == "__main__":
    app = App()
    app.run()
