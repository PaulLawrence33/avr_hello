#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

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

ISR(TIMER2_COMPA_vect, ISR_BLOCK)
{
	static int seconds = 0;
	static int one_twenty_fifths = 0;

	if(++one_twenty_fifths != 125)
		return;

	one_twenty_fifths = 0;
	++seconds;
	PORTB = digits[seconds % (sizeof(digits)/sizeof(*digits))];
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

	sei();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	for(;;)
		sleep_mode();
}
