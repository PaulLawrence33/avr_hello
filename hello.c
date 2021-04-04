#include <avr/io.h>
#include <avr/sleep.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#define OLED_ADDR 0x78

void error(int code)
{
	DDRB |= 1 << PB3;
	
	for(uint8_t i = 0; i < code * 2; ++i) {
		PORTB ^= 1 << PB3;
		_delay_ms(100);
	}

	sleep_enable();
	sleep_cpu();
}

int i2c_status(int flags)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | flags;
	while(!(TWCR & (1 << TWINT)));
	return TWSR & 0xf8;
}

void i2c_start()
{
	if (i2c_status(1 << TWSTA) != 8)
		error(1);

	TWDR = OLED_ADDR;
	if (i2c_status(0) != 0x18)
		error(2);
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	while(TWCR & (1 << TWSTO));
}

void i2c_send(uint8_t byte)
{
	TWDR = byte;
	if (i2c_status(0) != 0x28)
		error(3);
}

void lcd_command(uint8_t command)
{
	i2c_start();
	i2c_send(0x00);
	i2c_send(command);
	i2c_stop();
}

void lcd_command1(uint8_t command, uint8_t data)
{
	i2c_start();
	i2c_send(0x00);
	i2c_send(command);
	i2c_send(data);
	i2c_stop();
}

void lcd_data()
{
	i2c_start();
	i2c_send(0x40);
	i2c_send(0xff);
	i2c_send(0x40);
	i2c_send(0x00);
	i2c_stop();
}

int main()
{
	// Set clock speed to 8MHz
	CLKPR = (1 << CLKPCE); // enable a change to CLKPR
	CLKPR = 0; // set the CLKDIV to 0 - was 0011b = div by 8 taking 8MHz to 1MHz

	// Set i2c speed to 100kHz
	TWSR = 0;
	TWBR = 32;

	lcd_command1(0x8d, 0x14);
	lcd_command(0xaf);
	lcd_command(0xa4);

	for(int i = 0; i < 4096; ++i)
		lcd_data();

	return 0;
}
