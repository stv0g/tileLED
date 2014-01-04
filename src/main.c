#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "font.h"
#include "ds1820.h"

#define BUFLEN (8*64)

const char *msg = "Frohe Weihnachten und einen guten Rutsch wuenscht euch Steffen";

volatile float temp_cur;
volatile float temp_min = 1e6;
volatile float temp_max = -1e6;

volatile uint8_t buffer[BUFLEN] = { 0 };
volatile uint8_t scroll = 0;
volatile uint8_t len = 0;


void set_buffer(char* text) {
	uint8_t a = 0, b, c;

	for (c = 0; c < 8; c++)
		buffer[a++] = 0;

	for (b = 0; text[b]; b++) {
		for (c = 0; a < BUFLEN && c < FONT_WIDTH && font[text[b] - ' '][c] != 0xff; c++, a++) {
			buffer[a] = font[text[b] - ' '][c];
		}

		buffer[a++] = 0;
	}

	len = a;

	for (c = 0; c < 8; c++)
		buffer[a++] = 0;
}

void init(void) {
	/* IO Ports */
	DDRB = 0xc0; /* cathodes 6..7 */
	DDRC = 0x3f; /* cathodes 0..5 */
	DDRD = 0xff; /* anodes  */

	/* Timer2 */
	TCCR2 |= (1 << WGM21) | (1 << CS22) | (1 << CS21); /* prescaler = 256 */
	TIFR |= (1 << OCF2);
	OCR2 = 90; /* interrupt = ??? Hz */

	/* Timer1 */
	TCCR1A = 0;
	TCCR1B = (1 << CS12) | (1 << CS10); /* prescaler = 1024 => 0,12 Hz = 8,3s */
	TIFR |= (1 << OCF1A);
	OCR1A = (1 << 13);
}

ISR(TIMER2_COMP_vect) {
	static uint8_t cnt = 100;
	uint8_t row = cnt % 8;

	PORTD = 0;

	PORTB |= 0xC0;
	PORTB &= ~((1 << row) & 0xC0);
	PORTC = ~((1 << row) & 0x3f);

	PORTD = *(buffer+scroll+7-row);


	if (cnt) {
		cnt--;
	}
	else {
		if (scroll == len) {
			/* stop timer2 */
			TIMSK &= ~(1 << OCIE2);

			/* start timer1 */
			TCNT1 = 0;
			TIFR |= (1 << OCF1A);
			TIMSK |= (1 << OCIE1A);
		}
		else {
			scroll++;
		}

		cnt = 100;
	}
}

ISR(TIMER1_COMPA_vect) {
	static uint16_t cnt;
	static uint8_t step;
	char text[120];

	TIMSK &= ~(1 << OCIE1A); /* stop timer1 */

	if (cnt) {
		cnt--;
	}
	else {
		step++;
		step %= 6;

//		cnt = 300;
		scroll = 0;

		temp_cur = ds1820_read_temp(PB2);

		/* update minimum/maximum */
		if (temp_cur > temp_max) {
			temp_max = temp_cur;
		}
		if (temp_cur < temp_min) {
			temp_min = temp_cur;
		}

		switch (step) {
			case 0:
			case 2:
			case 4:
			case 6:
				snprintf(text, sizeof(text), "%.1f%cC", temp_cur, 0x80);
				set_buffer(text);
				break;
			case 1:
				snprintf(text, sizeof(text), "min %.1f%cC", temp_min, 0x80);
				set_buffer(text);
				break;
			case 3:
				snprintf(text, sizeof(text), "max %.1f%cC", temp_max, 0x80);
				set_buffer(text);
				break;
			case 5:
				set_buffer(msg);
				break;
		}


		TIFR |= (1 << OCF2);
		TIMSK |= (1 << OCIE2); /* start timer2 */
	}
}

int main(void) {
	init();
	ds1820_init(PB2);

//	set_buffer("hallo da");
	TIMSK |= (1 << OCIE2); /* start timer2 */

	sei();
	while (1);

	return 0;
}
