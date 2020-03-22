import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
# import matplotlib.dates as mdates
from itertools import count
import time
import datetime as dt
import numpy as np

plt.style.use('fivethirtyeight')
fig, (ax1, ax2) = plt.subplots(nrows=2, ncols=1, sharex=True)
# plt.subplots_adjust(left=0.1, bottom=0.5, right=0.2, top=0.6, wspace=0.2, hspace=0.2)
index = count()

pressure_data = open('pressure_metrics.txt','r').read()
pressure_lines = pressure_data.split('\n')
volume_data = open('volume_metrics.txt','r').read()
volume_lines = volume_data.split('\n')
# counter=0
xp = []
yp = []
xv = []
yv = []

def updatePressure(data):
    if len(data) > 1:
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
    if len(data) > 1:
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

def updateBoth(interval):
    updateVolume(volume_lines[next(index)])
    updatePressure(pressure_lines[next(index)])

ani = animation.FuncAnimation(fig,func=updateBoth, interval=1000)
plt.tight_layout()
plt.show()
