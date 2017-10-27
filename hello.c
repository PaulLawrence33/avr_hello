#include <avr/io.h>

#define F_CPU 1000000

#include <util/delay.h>

static const int digits[] = {
  0b11010111,
  0b00000011,
  0b11100110,
  0b10100111,
  0b00110011,
  0b10110101,
  0b11110101,
  0b00000111,
  0b11110111,
  0b00110111,
};

int main(int argc, char** argv)
{
	int i;
	DDRB = 0xff;

	for(;;) {
 		for (i = 0; i < sizeof(digits)/sizeof(*digits); ++i) {
			PORTB = digits[i];
			_delay_ms(500);
		}
	}
	return 0;
}
