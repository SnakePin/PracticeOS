OUT	= ../../out/kernel
SRC = .
AS			= nasm
LD			= ld
CC			= gcc
CFLAGS		= -m32 -ffreestanding -nostdlib -c -std=c11 -pedantic -o3 -Wall -gdwarf-2
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align

ASM_OBJS	= entry.o gdt.o interrupt_asm.o ioport.o
C_OBJS		= main.o vga.o interrupt_c.o utils.o
ASM_OBJ_TARGETS = $(addprefix $(OUT)/, $(ASM_OBJS))
C_OBJ_TARGETS = $(addprefix $(OUT)/, $(C_OBJS))

$(info $(shell mkdir -p $(OUT)))

.PHONY: all clean
all: $(OUT)/kernel.bin

$(ASM_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.s
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(C_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) "$<" -o "$@"

$(OUT)/kernel.elf: $(ASM_OBJ_TARGETS) $(C_OBJ_TARGETS)
	$(LD) $(LDFLAGS) -T kernel_linker.ld -o "$@" $(addprefix $(OUT)/, $^)

# This doesn't put the .bss in the output file, we should write an elf loader in the BL instead.
$(OUT)/kernel.bin: $(OUT)/kernel.elf
	objcopy -S -O binary "$<" "$@"

clean:
	rm -f $(OUT)/*
