MODE ?= fixed8.8

CC=../../../lang/micro-C/mc09/mc
AS=../../../lang/micro-C/as09/as09

all: a.out.srec

a.out.srec: crt0.s c.out
	$(AS) crt0.s -o $@ -l a.out.lst -v -O

c.out: $(MODE)/main.c
	$(CC) $<

clean:
	rm -f c.out a.out*
