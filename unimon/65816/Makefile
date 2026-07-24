#
# Makefile
#   Universal Monitor for W65C816S
#

.SUFFIXES: .asm .p .bin .hex .sr

devs :=	dev/dev_6850.asm

all:	unimon_65816.bin unimon_65816.hex

.p.bin:
	p2bin -v $*.p $*.bin

.p.hex:
	p2hex -F Intel $*.p $*.hex

.p.sr:
	p2hex -F Moto $*.p $*.sr

.asm.p:	config.inc $(devs)
	asl -L $*.asm

config.inc:
	[ -e config.inc ] || ln -s config/config.inc config.inc

clean:
	rm -f *.p *.bin *.hex *.sr *.lst
