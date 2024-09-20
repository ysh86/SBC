#
# Makefile
#   Universal Monitor for Z80
#

AS := ../../lang/asm/asl/bin/asl.exe
P2HEX := ../../lang/asm/asl/bin/p2hex.exe

.SUFFIXES: .asm .p .hex .sr

devs :=	unimon_z80.asm

all:	unimon_z80.hex

.p.hex:
	$(P2HEX) -F Intel $*.p $*.hex

.p.sr:
	$(P2HEX) -F Moto $*.p $*.sr

.asm.p:	config.inc $(devs)
	$(AS) -L $*.asm

config.inc:
	[ -e config.inc ] || ln -s config/config.inc config.inc

clean:
	rm -f *.p *.hex *.sr *.lst
