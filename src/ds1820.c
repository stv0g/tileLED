/**
 * Library to use DS18x20 with ATMEL Atmega family.
 *
 * For short DS18x20 wires there is no need for an external pullup resistor.
 * If the wire length exceeds one meter you should use a 4.7k pullup resistor 
 * on the data line. This library does not work for parasite power. 
 * You can just use one DS18x20 per Atmega Pin.
 *
 * @copyrigth 2010, Stefan Sicklinger
 * @url http://www.sicklinger.com
 * @author Stefan Sicklinger
 * @author Steffen Vogel
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ds1820.h"

uint8_t ds1820_reset(uint8_t pin) {
	uint8_t err = 100;

	DS1820_DDR |= (1 << pin);
	DS1820_PORT &= ~(1 << pin);
	_delay_us(480);

	DS1820_DDR &= ~(1 << pin);
	DS1820_PORT |= 1 << pin;
	_delay_us(66);

	err = (DS1820_PIN & (1 << pin)) >> pin;
	_delay_us(240);

	if ((DS1820_PIN & (1 << pin)) == 0) {
		err = 2;
	}

	return err;
}
void ds1820_wr_bit(uint8_t bit, uint8_t pin) {
	if (bit) {
		DS1820_DDR |= (1 << pin);
		DS1820_PORT &= ~(1 << pin);
		_delay_us(10);
		DS1820_DDR &= ~(1 << pin);
  		DS1820_PORT |= (1 << pin);
		_delay_us(54);
	}
	else {
		DS1820_DDR |= (1 << pin);
		DS1820_PORT &= ~(1 << pin);
		_delay_us(60);
		DS1820_DDR &= ~(1 << pin);
  		DS1820_PORT |= (1 << pin);
		_delay_us(4);
	}
}
uint8_t ds1820_re_bit(uint8_t pin) {
	uint8_t rebit;

	DS1820_DDR |= 1<<pin;
  	DS1820_PORT &= ~(1<<pin);
	_delay_us(1);

	DS1820_DDR &= ~(1<<pin);
	DS1820_PORT |= 1<<pin;
	_delay_us(10);

	rebit = (DS1820_PIN & (1 << pin)) >> pin;
	_delay_us(50);

	return rebit;
}

uint8_t ds1820_re_byte(uint8_t pin) {
	uint8_t byte = 0;
	uint8_t i;

	for (i = 0; i < 8; i++) {
		if (ds1820_re_bit(pin)) {
			byte |= (1 << i);
		}

		//delay_us(2); // be on the save side
	}

	return byte;
}
void ds1820_wr_byte(uint8_t byte,uint8_t pin) {
	uint8_t i;

	for (i = 0; i < 8; i++) { // writes byte, one bit at a time
		ds1820_wr_bit((byte & (1 << i)), pin);
	}

	_delay_us(5);
}

float ds1820_read_temp(uint8_t pin) {
	uint8_t error,i;
	uint16_t j = 0;
	uint8_t scratchpad[9] = { 0 };
	float temp = 0;

	error = ds1820_reset(pin);					// 1. Reset

	if (!error) {
		ds1820_wr_byte(0xCC, pin);  				// 2. skip ROM
		ds1820_wr_byte(0x44, pin);  				// 3. ask for temperature conversion

		while (ds1820_re_byte(pin) == 0xFF && j < 1000){	// 4. wait until conversion is finished 
			_delay_us(1);
			j++;
		}

		error = ds1820_reset(pin);				// 5. Reset
		ds1820_wr_byte(0xCC, pin);  				// 6. skip ROM
		ds1820_wr_byte(0xBE, pin);  				// 7. Read entire scratchpad 9 bytes

		for (i = 0; i < 9; i++) {	 			// 8. Get scratchpad byte by byte
		   scratchpad[i] = ds1820_re_byte(pin); 		// 9. read one DS18S20 byte
		}
	}

	if (scratchpad[1] == 0x00 && scratchpad[7]) {			// Value positive
		temp = (scratchpad[0] >>= 1) - 0.25f + (((float) scratchpad[7] - (float) scratchpad[6]) / (float) scratchpad[7]);
		temp = floor(temp * 10.0 + 0.5) / 10;			// Round value .x
	}

	if (scratchpad[1] != 0x00){					// Value negative
		temp = -((~scratchpad[0] >> 1) + 1);

		if (scratchpad[0] & 1){
			temp += 0.5;
		}
	}

	return temp;
}

void  ds1820_init(uint8_t pin) {
	uint16_t i = 0;
	uint8_t error = ds1820_reset(pin);				// 1. Reset

	if (!error) {
		ds1820_wr_byte(0xCC,pin);				// 2. skip ROM
		ds1820_wr_byte(0x44,pin);				// 3. ask for temperature conversion

		while (ds1820_re_byte(pin) == 0xFF && i < 1000) {	// 4. wait until conversion is finished 
			_delay_us(1);
			i++;
		}

		error=ds1820_reset(pin);				// 5. Reset
		ds1820_wr_byte(0xCC,pin);				// 6. skip ROM
		ds1820_wr_byte(0xBE,pin);				// 7. Read entire scratchpad 9 bytes
	}
}
