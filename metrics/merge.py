import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import time
import datetime as dt
import numpy as np
import serial
from itertools import count
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

from matplotlib.backends.backend_gtk3agg import (
    FigureCanvasGTK3Agg as FigureCanvas)
from gi.repository import Notify


def updateData(interval):
    data = list_data[next(index)]
    time.append(dt.datetime.now().strftime('%H:%M:%S'))
    y_pressure.append(float(data))
    y_volume.append(float(data))
    if len(time) > 10:
        time.pop(0)
        y_pressure.pop(0)
        y_volume.pop(0)
        ax_pressure.clear()
        ax_volume.clear()
    if data > 8:
        Notify.Notification.new("Max pressure Alert").show()
    plt.subplots_adjust(bottom=0.2)
    plt.xticks(rotation=45, ha='right')
    ax_pressure.plot(time, y_pressure)
    ax_volume.plot(time, y_volume)
    ax_volume.title.set_text('Volume (mL)')
    ax_pressure.title.set_text('Pressure (mbars)')


if __name__ == "__main__":    
    plt.style.use('seaborn')
    time = []
    y_pressure = []
    y_volume = []
    generate_rand_data()
    win = Gtk.Window()
    index = count()
    Notify.init("Apollo Notificator")
    win.connect("delete-event", Gtk.main_quit)
    win.set_default_size(1000, 800)
    win.set_title("Ventilator Metrics (pressure and volume)")
    shape, scale = 2., 2.
    list_data = np.random.gamma(shape, scale, 1000)    
    fig, (ax_pressure, ax_volume) = plt.subplots(nrows=2, ncols=1, sharex=True)
    sw = Gtk.ScrolledWindow()
    win.add(sw)
    sw.set_border_width(10)
    canvas = FigureCanvas(fig)
    canvas.set_size_request(800, 600)
    sw.add_with_viewport(canvas)
    ani = animation.FuncAnimation(fig,func=updateData, interval=1000)  
    win.show_all()
    Gtk.main()
