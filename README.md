tileLED
=======

A very minimalistic driver for 8x8 LED dot matrix displays.


Based on an ATmega8 this board can drive a simple LED matrix displays. There is no real "driver" circuit on the board. We just use the GPIOs of the AVR mcu to sink/source the LED current.

According to the ATmega8 datasheet this usage is a bit out of spec, but tests with 3 Volt button cells were successful. Also 5 Volt supplies are okay.

The source code includes a simple demonstration application. Paired with a DS1820 digital temperature sensor, the display shows the current temperature and records the minimal and maximal temperature ever measured.

A Blog post on http://www.steffenvogel.de will follow.
