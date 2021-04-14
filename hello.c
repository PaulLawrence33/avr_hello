#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "constants.h"
#include <util/delay.h>

#include "lcd.h"

volatile uint8_t hours = 12, mins, secs;

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

	// TIMER1 interrupt once a second
	TCCR1B |= 1 << WGM12 | 1 << CS10 | 1 << CS12;
	TIMSK1 |= 1 << OCIE1A;
	OCR1A = 7812;

	// Enable interrupts for switches
	PCICR |= 1 << PCIE1;
	PCMSK1 |= 1 << PCINT9;
	PORTC |= 1 << PC1;

	DDRB |= PB3 << 1;

	sleep_enable();
	for(;;) {
		sei();
		sleep_cpu();
		cli();
		lcd_printf_large(0, 2, "%2d:%02d", hours, mins);
		lcd_printf(5 * 18 + 4, 3, "%02d", secs);
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (++secs != 60)
		return;
	secs = 0;
	if (++mins != 60)
		return;
	mins = 0;
	if (++hours != 13)
		return;
	hours = 1;
}

ISR(PCINT1_vect)
{
	if (PINC & 1 << PC1)
		PORTB &= ~(1 << PB3);
	else
		PORTB |= 1 << PB3;
}
