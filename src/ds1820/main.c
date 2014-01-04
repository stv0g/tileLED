/**
 * DS1820 test
 *
 * @author	Steffen Vogel
 * @copyright	2013 Institute for Man-Machine Interaction, RWTH Aachen
 * @license	http://www.gnu.org/licenses/gpl.txt GNU Public License
 * @author	Steffen Vogel <post@steffenvogel.de>
 * @link	http://www.mmi.rwth-aachen.de/?microcontrollerag
 */
/*
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 */

#include <stdio.h>

#include "lcd.h"
#include "ds1820.h"

int main() {
	float temp;
	char buffer[16];


	lcd_init(LCD_DISP_ON);
	ds1820_init(PB2);

	lcd_puts("DS1820\nReadout");
	_delay_ms(1000);
	lcd_clrscr();


	for (;;) {
		temp = ds1820_read_temp(PB2);
		snprintf(buffer, sizeof(buffer), "%.2f ", temp);

		lcd_home();
		lcd_puts(buffer);
		lcd_putc(0xdf);
		lcd_putc('C');

		_delay_ms(500);
	}

	return 0;
}
