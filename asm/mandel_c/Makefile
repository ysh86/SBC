EXEC=mandel

CC=c68
LD=/usr/lib/ld
CV=/usr/lib/cv
ASFLAGS=-c -DACK -T.
AS=$(CC) $(ASFLAGS)
CFLAGS=-c -O -D_POSIX_SOURCE -D__STDC__=1 -T.
LFLAGS=-c
# LDFLAGS+=-a0:2 -a1:2 -a2:2 -a3:2
LIB=/usr/lib

all:	$(EXEC)

$(EXEC):	$(EXEC).o $(LIB)/libc.a $(LIB)/head.o
	$(LD) -o $(EXEC).ack $(LFLAGS) $(LIB)/head.o $(EXEC).o \
		$(LIB)/libc.a $(LIB)/end.o
	$(CV) $(EXEC).ack $(EXEC).mix
	@rm -f $(EXEC).ack
	@cp $(EXEC).mix $@
	@strip $@

clean:
	rm -f *.o *.bak *.mix $(EXEC)
