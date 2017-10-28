hello.hex: hello.elf
	avr-objcopy -j .text -j .data -O ihex hello.elf hello.hex

hello.asm: hello.elf
	avr-objdump -d hello.elf > hello.asm

hello.elf: hello.o
	avr-gcc -g -mmcu=atmega88 -o hello.elf hello.o

hello.o: hello.c
	avr-gcc -g -std=c11 -Os -mmcu=atmega88 -c hello.c

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.hex
