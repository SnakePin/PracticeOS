OUTPREFIX	= ../../out/kernel
AS			= nasm
LD			= ld
CC			= gcc
CFLAGS		= -m32 -ffreestanding -nostdlib -g -c -std=c11 -pedantic -o3
ASFLAGS		= -f elf -F stabs -g
LDFLAGS		= 
ASM_OBJS	= entry.o
C_OBJS		= main.o

$(info $(shell mkdir -p $(OUTPREFIX)))

all: --build convert_elf
--build: kernel.elf

$(ASM_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

$(C_OBJS): %.o: %.c
	$(CC) $(CFLAGS) "$<" -o "$(OUTPREFIX)/$@"

kernel.elf: entry.o main.o
	$(LD) $(LDFLAGS) -T kernel_linker.ld -o "$(OUTPREFIX)/$@" $(addprefix $(OUTPREFIX)/, $^)

convert_elf: --build
	objcopy -S -O binary "$(OUTPREFIX)/kernel.elf" "$(OUTPREFIX)/kernel.bin"

clean:
	rm -f $(OUTPREFIX)/*
