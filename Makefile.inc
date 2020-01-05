# Primary target
.PHONY: main

# Make target to create asm output
.PHONY: asm

# Make target to clean directory
.PHONY: clean

# Make target to flash board
.PHONY: flash

# All our rules
main: $(obj).hex

asm: $(obj).asm

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

%.asm: %.elf
	avr-objdump -d $< > $@

%.elf: %.o
	$(cc) -o $@ $<

%.o: %.c
	$(cc) -std=c11 -Os -c $<

clean:
	rm -f *.o *.elf *.hex *.asm

flash: main
	avrdude -c usbtiny -p $(partno) -U flash:w:$(obj).hex

# Preflight tests
preflight := $(or $(SETAVR), $(error "Run '. setavr.sh' before make"))

# All our variables
cc = avr-gcc -g -mmcu=$(target) -Wall -Wextra -Werror
