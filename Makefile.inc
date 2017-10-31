# Primary target
.PHONY: main

# Make target to create asm output
.PHONY: asm

# Make target to clean directory
.PHONY: clean

# All our rules
main: $(obj).hex preflight

asm: $(obj).asm preflight

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

%.asm: %.elf
	avr-objdump -d $< > $@

%.elf: %.o
	$(cc) -o $@ $<

%.o: %.c
	$(cc) -std=c11 -Os -c $<

.PHONY: preflight

preflight:
ifneq ("$(SETAVR)","1")
	$(error "Run '. setavr.sh' before running make")
endif

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.hex
	rm -f *.asm

# All our variables
cc = avr-gcc -g -mmcu=$(target)
