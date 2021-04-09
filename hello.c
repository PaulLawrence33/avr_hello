#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "constants.h"
#include <util/delay.h>

#include "lcd.h"

volatile int counter = 0;

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

	// TIMER1 interrupt once a second
	TCCR1B |= 1 << WGM12 | 1 << CS10 | 1 << CS12;
	TIMSK1 |= 1 << OCIE1A;
	OCR1A = 7812;

	sleep_enable();
	sei();
	for(;;) {
		sleep_cpu();
		lcd_printf_large(0, 2, "\"%d\"", counter);
	}
}

ISR(TIMER1_COMPA_vect)
{
	counter++;
}
