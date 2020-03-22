import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
from itertools import count
import time
import datetime as dt
import numpy as np
import serial
import argparse

plt.style.use('fivethirtyeight')
fig, (ax1, ax2) = plt.subplots(nrows=2, ncols=1, sharex=True)
index = count()

xp = []
yp = []
xv = []
yv = []

def updatePressure(data):
    xp.append(dt.datetime.now().strftime('%H:%M:%S'))
    yp.append(float(data))
    # print(xp)
    if len(xp) > 10:
        xp.pop(0)
        yp.pop(0)
        # ax1.clear()
    plt.subplots_adjust(bottom=0.2)
    plt.xticks(rotation=45, ha='right')
    ax1.plot(xp, yp)
    plt.title('Ventilator Metrics (pressure and volume)')
    ax1.title.set_text('Pressure (mbars)')


def updateVolume(data):
    xv.append(dt.datetime.now().strftime('%H:%M:%S'))
    yv.append(float(data))
    # print(xv)
    if len(xv) > 10:
        xv.pop(0)
        yv.pop(0)
        # ax2.clear()
    ax2.plot(xv, yv)
    plt.xticks(rotation=45, ha='right')
    plt.xlabel('Time')
    ax2.title.set_text('Volume (mL)')

def updateViaSerialPort(interval):
    data = serial.readline()
    updatePressure(data.decode('utf-8'))

def updateBothRand(interval):
    updateVolume(np.random.rand(1)[0])
    updatePressure(np.random.rand(1)[0])

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--testdata', action='store_true', help='Random values to test graphics')
    parser.add_argument('--serialport', nargs='?', const='/dev/ttyUSB0', help='Serial port name')
    args = parser.parse_args()
    if not args.testdata:
        serial = serial.Serial(args.serialport)

    ani = animation.FuncAnimation(fig,func=updateBothRand, interval=1000) if args.testdata else animation.FuncAnimation(fig,func=updateViaSerialPort, interval=1000)
    plt.tight_layout()
    plt.show()
