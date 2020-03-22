#!/usr/bin/env python
import gi
gi.require_version('Gtk', '3.0')

from gi.repository import Gtk
from matplotlib.backends.backend_gtk3agg import (
    FigureCanvasGTK3Agg as FigureCanvas)
from matplotlib.figure import Figure
import numpy as np


__path__ = "/".join(__file__.split('/')[0:-1])


@Gtk.Template(filename='device.ui')
class Device(Gtk.Grid):
    __gtype_name__ = 'Device'

    draw_surface = Gtk.Template.Child()

    def __init__(self, *args, **kwargs):
        super(Device, self).__init__(*args, **kwargs)
        f = Figure(figsize=(5, 4), dpi=100)
        self.plot_1 = f.add_subplot(3, 1, 1)
        self.plot_2 = f.add_subplot(3, 1, 2)
        self.plot_3 = f.add_subplot(3, 1, 3)

        t = np.arange(0.0, 3.0, 0.01)
        s = np.sin(2 * np.pi * t)

        self.plot_1.plot(t, s)
        self.plot_2.plot(t, s)
        self.plot_3.plot(t, s)

        # A scrolled window border goes outside the scrollbars and viewport
        self.draw_surface.set_border_width(10)

        canvas = FigureCanvas(f)  # a Gtk.DrawingArea
        canvas.set_size_request(800, 600)

        self.draw_surface.add_with_viewport(canvas)


def add_device_log(s: str):
    pass


def add_device_widget_to_notebook():
    obj_notebook = app_builder.get_object("tab_panel")

    page_label = Gtk.Label(label='New Device')
    page_object = Device()

    obj_notebook.append_page(page_object, tab_label=page_label)
    app_window.show_all()


def handle_device_connection(*args, **kwargs):
    add_device_widget_to_notebook()
    add_device_log("Dispositivo conectado")


if __name__ == '__main__':
    app_builder = Gtk.Builder()
    app_builder.add_from_file('gui.glade')

    app_events = {
        'on_click_btn_device_connect': handle_device_connection
    }

    app_builder.connect_signals(app_events)
    app_window = app_builder.get_object("App")
    app_window.show_all()

    Gtk.main()
