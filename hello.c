#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

// a
//f b
// g
//e c
// d .

static const int digits[] = {
  //.gfedcba 
  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01100111,
};

const int displays = 2;

ISR(TIMER2_COMPA_vect, ISR_BLOCK)
{
	static int seconds = 0;
	static int one_twenty_fifths = 0;
	static int digit = 0;
	static char segments[2];

	digit = (digit + 1) % displays;
	PORTC |= 0x0f;
	PORTB = segments[digit];
	PORTC &= 0xff ^ (1 << digit); 
	
	if (one_twenty_fifths == 62) {
		segments[1] &= 0x7f;
		segments[0] |= 0x80;
	}

	if(++one_twenty_fifths != 125)
		return;

	one_twenty_fifths = 0;

	++seconds;
	segments[1] = digits[seconds % 10] | 0x80;
	segments[0] = digits[seconds / 10 % 10];
}

// initialize timer, interrupt and variable
void timer2_init()
{
	// Set up timer in CTC mode
	TCCR2A = (1 << WGM21);

	// Set prescaler to 64
	TCCR2B = (1 << CS22);
  
	// Initialize counter
	TCNT2 = 0;
  
	// Initialize compare value
	OCR2A = 125;
  
	// Enable compare interrupt
	TIMSK2 |= (1 << OCIE1A);
}

int main(int argc, char** argv)
{
	timer2_init();
	DDRB = 0xff;
	DDRC = 0xff;

	PORTC = 0x00;

	sei();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	for(;;)
		sleep_mode();
}
