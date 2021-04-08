#include <avr/io.h>

#include "constants.h"
#include <util/delay.h>

#include "lcd.h"

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
	for (int i = 0;; ++i) {
		lcd_printf_large(0, 2, "\"%d\"", i);
		_delay_ms(250);
	}
	return 0;
}
