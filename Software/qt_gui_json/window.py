from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *


class MainWindow(QWidget):
    
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
        self.layout = QGridLayout(self)