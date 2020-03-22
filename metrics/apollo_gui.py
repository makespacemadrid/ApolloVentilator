import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

from matplotlib.backends.backend_gtk3agg import (
    FigureCanvasGTK3Agg as FigureCanvas)
from matplotlib.figure import Figure
import numpy as np


class MyWindow(Gtk.Window):

    def __init__(self):
        Gtk.Window.__init__(self, title="Hello World")
        self.set_size_request(1000, 600)

        self.box = Gtk.Box(spacing=6)

        self.left_panel = Gtk.Box(spacing=2, expand=True)
        self.right_panel = Gtk.Box(spacing=2, orientation=Gtk.Orientation.VERTICAL)

        self.add(self.box)

        self.box.add(self.left_panel)
        self.box.add(self.right_panel)

        f = Figure(figsize=(5, 4), dpi=100)
        self.plot_1 = f.add_subplot(3, 1, 1)
        self.plot_2 = f.add_subplot(3, 1, 2)
        self.plot_3 = f.add_subplot(3, 1, 3)

        t = np.arange(0.0, 3.0, 0.01)
        s = np.sin(2 * np.pi * t)

        self.plot_1.plot(t, s)
        self.plot_2.plot(t, s)
        self.plot_3.plot(t, s)

        sw = Gtk.ScrolledWindow(expand=True)
        self.left_panel.add(sw)
        # A scrolled window border goes outside the scrollbars and viewport
        sw.set_border_width(10)

        canvas = FigureCanvas(f)  # a Gtk.DrawingArea
        canvas.set_size_request(800, 600)
        sw.add_with_viewport(canvas)

        self.entry = Gtk.Entry()
        self.entry.set_text("Hello World")
        self.right_panel.pack_start(self.entry, True, True, 0)

        self.button1 = Gtk.Button(label="Standby")
        self.button1.connect("clicked", self.on_button1_clicked)
        self.right_panel.pack_start(self.button1, True, True, 0)

        self.button2 = Gtk.Button(label="Goodbye")
        self.button2.connect("clicked", self.on_button2_clicked)
        self.right_panel.pack_start(self.button2, True, True, 0)

    def on_button1_clicked(self, widget):
        t = np.arange(0.0, 6.0, 0.01)
        s = np.sin(1/3 * np.pi * t)
        self.plot_3.clear()
        self.plot_3.plot(t, s)

    def on_button2_clicked(self, widget):
        print("Goodbye")


win = MyWindow()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()