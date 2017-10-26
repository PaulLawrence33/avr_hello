#include <avr/io.h>

#define F_CPU 1000000

#include <util/delay.h>

int main(int argc, char** argv)
{
	DDRB = 0xff;

	for(;;) {
		PORTB = 0xff;
		_delay_ms(1000);
		PORTB = 0x00;
		_delay_ms(1000);
	}
	return 0;
}
