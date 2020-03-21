import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
# import matplotlib.dates as mdates
# from itertools import count
import datetime as dt
import numpy as np

plt.style.use('fivethirtyeight')
fig, (ax1, ax2) = plt.subplots(nrows=2, ncols=1, sharex=True)
# plt.subplots_adjust(left=0.1, bottom=0.5, right=0.2, top=0.6, wspace=0.2, hspace=0.2)
# index = count()

def updatePressure():
    graph_data = open('pressure_metrics.txt','r').read()
    lines = graph_data.split('\n')
    xs = []
    ys = []
    for line in lines:
        if len(line) > 1:
            x, y = line.split(',')
            xs.append(dt.datetime.now().strftime('%H:%M:%S.%f'))
            # xs.append(plt.gcf().autofmt_xdate()))
            ys.append(float(y))
    ax1.clear()
    plt.subplots_adjust(bottom=0.2)
    ax1.plot(xs, ys)

    # plt.title('Ventilator Metrics (pressure and volume)')

    ax1.title.set_text('Pressure (mbars)')


def updateVolume():
    graph_data = open('volume_metrics.txt','r').read()
    lines = graph_data.split('\n')
    xs = []
    ys = []
    for line in lines:
        if len(line) > 1:
            x, y = line.split(',')
            xs.append(dt.datetime.now().strftime('%H:%M:%S%f'))
            ys.append(float(y))
    ax2.clear()
    ax2.plot(xs, ys)
    plt.xticks(rotation=45, ha='right')
    plt.xlabel('Time')
    ax2.title.set_text('Volume (mL)')


def updateBoth(interval):
    updateVolume()
    updatePressure()

ani = animation.FuncAnimation(fig, updateBoth, interval=1000)
plt.tight_layout()
plt.show()
