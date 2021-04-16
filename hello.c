#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include <stdbool.h>

#include "constants.h"
#include <util/delay.h>

#include "lcd.h"

volatile enum state {
	normal,
	flashing_hours,
	flashing_minutes,
	flashing_seconds,
} state = normal;

volatile enum b1_state {
	released,
	pressed,
	pressed_tick1,
} b1_state = released;

volatile bool half_second = false;
volatile uint8_t hours = 12, minutes = 0, seconds = 0;

double get_temperature()
{
	ADCSRA |= 1 << ADSC;
	while(ADCSRA & 1 << ADSC);

	double volts = ADCW * 1.1 / 1024;
	return volts;
	double centigrade = volts * 100 - 50;
	return centigrade;
}

int main()
{
	// Set clock speed to 8MHz
	CLKPR = (1 << CLKPCE); // enable a change to CLKPR
	CLKPR = 0; // set the CLKDIV to 0 - was 0011b = div by 8 taking 8MHz to 1MHz

	// Set i2c speed to 100kHz
	TWSR = 0;
	TWBR = 32;

	lcd_init();
	lcd_clear();

	// Initialize temperature sensor readings
	ADMUX = 3 << REFS0 | 0b0000 << MUX0;
	ADCSRA |= 1 << ADEN | 6 << ADPS0;

	// TIMER1 interrupt twice a second
	TCCR1B |= 1 << WGM12 | 1 << CS12;
	TIMSK1 |= 1 << OCIE1A;
	OCR1A = 15625;

	// Enable interrupts for switches
	PCICR |= 1 << PCIE1;
	PCMSK1 |= 1 << PCINT9 | 1 << PCINT10 | 1 << PCINT11;
	PORTC |= 1 << PC1 | 1 << PC2 | 1 << PC3;

	DDRB |= PB3 << 1;

	sleep_enable();
	for(;;) {
		sei();
		sleep_cpu();
		cli();

		if (half_second || state == normal || state == flashing_seconds)
			lcd_printf_large(0, 2, "%2d:%02d", hours, minutes);
		else if (state == flashing_hours)
			lcd_printf_large(0, 2, "  :%02d", minutes);
		else if (state == flashing_minutes)
			lcd_printf_large(0, 2, "%2d:  ", hours);

		if (half_second || state != flashing_seconds)
			lcd_printf(5 * 18 + 4, 3, "%02d", seconds);
		else
			lcd_printf(5 * 18 + 4, 3, "  ");
	}
}

ISR(TIMER1_COMPA_vect)
{
	half_second = !half_second;

	switch (b1_state) {
		case pressed:
			b1_state = pressed_tick1;
			break;

		case pressed_tick1:
			b1_state = released;
			if (state == normal)
				state = flashing_hours;
			else
				state = normal;

		default:
			break;
	}

	if (!half_second) {
		if (++seconds != 60)
			return;
		seconds = 0;
		if (++minutes != 60)
			return;
		minutes = 0;
		if (++hours != 13)
			return;
		hours = 1;
	}
}

ISR(PCINT1_vect)
{
	if (!(PINC & 1 << PC2)) {
		switch (state) {
			case flashing_hours: hours++; if (hours == 13) hours = 1; break;
			case flashing_minutes: minutes++; if (minutes == 60) minutes = 0; break;
			case flashing_seconds: seconds = 0; break;
			default: break;
		}
	}

	if (!(PINC & 1 << PC1)) {
		switch (state) {
			case flashing_hours: hours--; if (hours == 0) hours = 12; break;
			case flashing_minutes: minutes--; if (minutes == 255) minutes = 59; break;
			case flashing_seconds: seconds = 0; break;
			default: break;
		}
	}

	if (PINC & 1 << PC3) {
		b1_state = released;
	} else {
		b1_state = pressed;

		switch(state) {
			case flashing_hours:   state = flashing_minutes; break;
			case flashing_minutes: state = flashing_seconds; break;
			case flashing_seconds: state = flashing_hours;   break;
			default: break;
		}
	}
}
