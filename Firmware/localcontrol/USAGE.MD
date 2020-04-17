# Local control to Ventilator

This firmware allow to control an Apollo Ventilator using external arduino with the local controls

The conectivity between it's using serial port, in Arduino Uno you need use ports 0(RX)-1(TX) to ports 14(TX3)-15(RX3) in Arduino Mega

This Arduino Uno need conected an Lcd display using I2C and one encoder in ports A0,A1,A2 (you can update this in `include/local.env.h`)