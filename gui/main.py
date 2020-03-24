#!/usr/bin/env python
"""

"""
import argparse
import threading

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, GLib, Pango


from matplotlib.backends.backend_gtk3agg import (
    FigureCanvasGTK3Agg as FigureCanvas
)
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import datetime as dt
import numpy as np
import time

__path__ = "/".join(__file__.split('/')[0:-1])

gl_app = None
gl_app_builder = None
gl_app_window = None


@Gtk.Template(filename='device.ui')
class Device(Gtk.Grid):
    __gtype_name__ = 'Device'

    cte_interval_animation = 1000

    draw_surface: Gtk.ScrolledWindow = Gtk.Template.Child()

    btn_toggle_standby: Gtk.ToggleButton = Gtk.Template.Child()
    btn_send_configuration: Gtk.Button = Gtk.Template.Child()

    entry_fio2: Gtk.Entry = Gtk.Template.Child()
    entry_breath_per_minute = Gtk.Template.Child()
    entry_current_volume: Gtk.Entry = Gtk.Template.Child()
    entry_peep: Gtk.Entry = Gtk.Template.Child()

    def __init__(self, parent_window, dev_args, *args, **kwargs):
        super(Device, self).__init__(*args, **kwargs)

        self.btn_toggle_standby.connect('toggled', self.event_toggle_standby)
        self.btn_send_configuration.connect('clicked', self.event_send_configuration)

        font_config = Pango.FontDescription('Dejavu Sans Mono 40')

        self.entry_fio2.modify_font(font_config)
        self.entry_breath_per_minute.modify_font(font_config)
        self.entry_current_volume.modify_font(font_config)
        self.entry_peep.modify_font(font_config)

        dev_args = kwargs.get('dev_args', {
            'serial_object': None
        })

        self.parent_window = parent_window
        self.serial_port = dev_args['serial_object']

        self.connection_datetime = dt.datetime.now()

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
        self.parent_window.show_all()
        canvas.show()

        self.thread_serial_port = threading.Thread(target=self.loop_serial_read)
        self.thread_serial_port.daemon = True
        self.thread_serial_port.start()

    def event_toggle_standby(self, ctrl):
        # TODO: self.serial_port.send("STANDBY_CMD")
        print("Standby Toggle")

    def event_send_configuration(self, ctrl):
        # TODO: get_entry_values -> self.entry_fio2.get_text() -> str con el valor o cadena vacia
        # TODO: serial_port.send("CONFIG_CMD + VALUES")
        print("Configuration Sended")

    def loop_plot_update(self, interval):
        plt.subplots_adjust(bottom=0.2)
        plt.xticks(rotation=45, ha='right')
        self.plot_ax_volume.plot(self.axis_x_time, self.axis_y_volume)
        self.plot_ax_pressure.plot(self.axis_x_time, self.axis_y_pressure)

    def loop_update_values(self, timestamp, pressure, volume):
        self.axis_x_time.append(timestamp)  # should point to data timeline in class
        self.axis_y_pressure.append(float(pressure))
        self.axis_y_volume.append(float(volume))

        if len(self.axis_x_time) > 10:
            self.axis_x_time.pop(0)
            self.axis_y_pressure.pop(0)
            self.axis_y_volume.pop(0)
            self.plot_ax_pressure.clear()
            self.plot_ax_volume.clear()

    def loop_serial_read(self):
        def parse_serial_line(parse_str):
            # TODO: MAES
            fake_data = ["DATA"] + [str(d) for d in np.random.rand(2).tolist()]  # should point to data object in class
            return fake_data

        while True:
            serial_line = "Whatever"  # TODO: Leer de puerto serie
            serial_data = parse_serial_line(serial_line)  # TODO: Usar funciones de Parser
            serial_command = serial_data[0]
            if serial_command == "DATA":
                timestamp = dt.datetime.now().strftime('%H:%M:%S')
                data = serial_data[1:3]
                args = [timestamp, data[0], data[1]]
                GLib.idle_add(self.loop_update_values, *args)
            elif serial_command == "ALARM":
                # TODO: Mostrar alarma con notificación
                pass
            time.sleep(1)


class App:
    def __init__(self, app_builder, app_window):
        self.builder = app_builder
        self.window = app_window
        self.devices = []

    def on_click_btn_device_connect(self, button):
        # TODO: Coger valor de puerto serie y etiqueta
        # TODO: Se llama a la lib pyserial y se intenta conectar
        # TODO: Si es correcto se llama a add_device_wid... con el objeto de puerto serie y demas (device_args)
        entry_label = self.builder.get_object('entry_device_label')
        entry_port = self.builder.get_object('entry_device_label')
        self.add_device_widget_to_notebook(device_args={
            'label': entry_label.get_text() or 'Paciente {}'.format(len(self.devices) + 1),
            'port': entry_port.get_text()
        })

    def add_device_widget_to_notebook(self, device_args: dict):
        obj_notebook: Gtk.Notebook = self.builder.get_object("tab_panel")

        page_label = Gtk.Label(label=device_args.get('label', 'Device'))
        page_object = Device(parent_window=self.window, dev_args=device_args)

        obj_notebook.append_page(page_object, tab_label=page_label)
        self.devices.append(Device)

        obj_notebook.set_current_page(len(self.devices))
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

