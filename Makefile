obj = hello
target = atmega88

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

%.asm: %.elf
	avr-objdump -d $< > $@

%.elf: %.o
	$(cc) -o $@ $<

%.o: %.c
	$(cc) -std=c11 -Os -c $<

cc = avr-gcc -g -mmcu=$(target)

$(obj).hex:

asm : $(obj).asm

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.hex
	rm -f *.asm

