#!/usr/bin/env python
"""

"""
import argparse

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

from matplotlib.backends.backend_gtk3agg import (
    FigureCanvasGTK3Agg as FigureCanvas
)
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import datetime as dt
import numpy as np


__path__ = "/".join(__file__.split('/')[0:-1])

gl_app = None
gl_app_builder = None
gl_app_window = None


@Gtk.Template(filename='device.ui')
class Device(Gtk.Grid):
    __gtype_name__ = 'Device'

    cte_interval_animation = 1000

    draw_surface: Gtk.ScrolledWindow = Gtk.Template.Child()

    def __init__(self, *args, **kwargs):
        super(Device, self).__init__(*args, **kwargs)

        self.axis_y_pressure = []
        self.axis_y_volume = []
        self.axis_x_time = []

        plt.style.use('seaborn')
        plot_figure, (ax_pressure, ax_volume) = plt.subplots(nrows=2, ncols=1, sharex=True)

        self.plot_figure = plot_figure
        self.plot_ax_pressure = ax_pressure
        self.plot_ax_volume = ax_volume

        self.plot_ax_volume.title.set_text('Volume (mL)')
        self.plot_ax_pressure.title.set_text('Pressure (mbars)')

        # A scrolled window border goes outside the scrollbars and viewport
        self.draw_surface.set_border_width(10)

        canvas = FigureCanvas(plot_figure)  # a Gtk.DrawingArea
        canvas.set_size_request(800, 600)

        self.plot_animation = animation.FuncAnimation(
            self.plot_figure,
            func=self.loop_plot_update,
            interval=self.cte_interval_animation
        )

        self.draw_surface.add_with_viewport(canvas)
        self.draw_surface.queue_draw()

    def loop_plot_update(self, interval):
        data = np.random.rand(2).tolist() # should point to data object in class
        self.axis_x_time.append(dt.datetime.now().strftime('%H:%M:%S')) # should point to data timeline in class
        self.axis_y_pressure.append(float(data[0]))
        self.axis_y_volume.append(float(data[1]))

        if len(self.axis_x_time) > 10:
            self.axis_x_time.pop(0)
            self.axis_y_pressure.pop(0)
            self.axis_y_volume.pop(0)
            self.plot_ax_pressure.clear()
            self.plot_ax_volume.clear()

        plt.subplots_adjust(bottom=0.2)
        plt.xticks(rotation=45, ha='right')
        self.plot_ax_volume.plot(self.axis_x_time, self.axis_y_volume)
        self.plot_ax_pressure.plot(self.axis_x_time, self.axis_y_pressure)



class App:
    def __init__(self, app_builder, app_window):
        self.builder = app_builder
        self.window = app_window

    def on_click_btn_device_connect(self, button):
        self.add_device_widget_to_notebook()

    def add_device_widget_to_notebook(self):
        obj_notebook = self.builder.get_object("tab_panel")

        page_label = Gtk.Label(label='New Device')
        page_object = Device()

        obj_notebook.append_page(page_object, tab_label=page_label)
        self.window.show_all()


def get_main_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--testdata', action='store_true', help='Random values to test graphics')
    parser.add_argument('--serialport', nargs='?', default='/dev/ttyUSB0', type=str, help='Serial port name')
    parser.add_argument('--show_seconds', nargs='?', default='10', type=int, help='Seconds shown in graphic')
    args = parser.parse_args()
    return args


def build_gtk_app():
    global gl_app_builder
    global gl_app_window

    gl_app_builder = Gtk.Builder()
    gl_app_builder.add_from_file('gui.ui')

    gl_app_window = gl_app_builder.get_object("App")

    gtk_app = App(gl_app_builder, gl_app_window)
    gl_app_builder.connect_signals(gtk_app)

    gl_app_window.connect("delete-event", Gtk.main_quit)

    return gtk_app


if __name__ == '__main__':
    app = build_gtk_app()

    app.window.set_title('Ventilator Metrics (pressure and volume)')
    app.window.show_all()
    app.window.maximize()

    Gtk.main()
