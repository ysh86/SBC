CROSS   = m68k-linux-gnu
AS      = $(CROSS)-as
CC      = $(CROSS)-gcc-14
LD      = $(CROSS)-ld
OBJCOPY = $(CROSS)-objcopy
OBJDUMP = $(CROSS)-objdump

CPU     = -m68000 -msoft-float
CFLAGS  = $(CPU) -O2 -ffreestanding -fno-builtin -Wall -Wextra
LDFLAGS = $(CPU) -nostdlib -T ram.ld \
          -Wl,--build-id=none \
          -Wl,--defsym=_ssp_top=$(SSP_TOP) \
          -Wl,--defsym=_usp_top=$(USP_TOP)

START_ADDR ?= 0x400
SSP_TOP ?= 0x7FFF0
USP_TOP ?= 0x7FF00

OBJS = crt0.o main.o

all: prog.bin

prog.elf: $(OBJS) ram.ld
	$(CC) $(LDFLAGS) $(OBJS) -lgcc -o $@

prog.bin: prog.elf
	$(OBJCOPY) -O binary $< $@

srec: prog.bin
	srec_cat $< -Binary -offset $(START_ADDR) -Execution_Start_Address $(START_ADDR) -Output prog.srec -Motorola -disable=header -disable=data-count -disable=footer

disasm: prog.elf
	$(OBJDUMP) -D -S $< > prog.lst

clean:
	rm -f *.o prog.elf prog.bin
