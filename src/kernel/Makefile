OUTPREFIX	= ../../out/kernel
AS			= nasm
LD			= ld
CC			= gcc
CFLAGS		= -m32 -ffreestanding -nostdlib -c -std=c11 -pedantic -o3 -Wall -gdwarf-2
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align
ASM_OBJS	= entry.o gdt.o interrupt_asm.o
C_OBJS		= main.o vga.o interrupt_c.o

$(info $(shell mkdir -p $(OUTPREFIX)))

all: --build convert_elf
--build: kernel.elf

$(ASM_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

$(C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) "$<" -o "$(OUTPREFIX)/$@"

kernel.elf: entry.o gdt.o interrupt_asm.o interrupt_c.o main.o vga.o
	$(LD) $(LDFLAGS) -T kernel_linker.ld -o "$(OUTPREFIX)/$@" $(addprefix $(OUTPREFIX)/, $^)

convert_elf: --build
	objcopy -S -O binary "$(OUTPREFIX)/kernel.elf" "$(OUTPREFIX)/kernel.bin"

clean:
	rm -f $(OUTPREFIX)/*
