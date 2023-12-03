OUT			= ../../out/kernel_loader
SRC			= .
AS			= nasm
LD			= ld
CC			= gcc
CFLAGS		= -m32 -ffreestanding -nostdlib -c -std=c11 -pedantic -Wall -gdwarf-2
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align -L../../out/shared/ -l:libshared.a
INCLUDE		= $(SRC)/include ../shared/include

ASM_OBJS	= entry.o int_gate16/int_gate16_asm.o jump_to_code.o
C_OBJS		= main.o int_gate16/int_gate16_c.o lba_disk.o memory.o
ASM_OBJ_TARGETS = $(addprefix $(OUT)/, $(ASM_OBJS))
C_OBJ_TARGETS = $(addprefix $(OUT)/, $(C_OBJS))

.PHONY: all clean ensure_path
all: $(OUT)/kernel_loader.bin

ensure_path:
	@mkdir -p $(OUT)
	@mkdir -p $(OUT)/int_gate16

clean:
	@rm -rf $(OUT)/* || true

$(ASM_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.s | ensure_path
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(C_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.c | ensure_path
	$(CC) $(CFLAGS) "$<" -o "$@" $(INCLUDE:%=-I%)

$(OUT)/kernel_loader.elf: $(ASM_OBJ_TARGETS) $(C_OBJ_TARGETS) | ensure_path
	$(LD) -T kernel_ldr_linker.ld -o "$@" $^ $(LDFLAGS)

# This doesn't put the .bss in the output file, we should write an elf loader in the BL instead.
$(OUT)/kernel_loader.bin: $(OUT)/kernel_loader.elf | ensure_path
	objcopy -S -O binary "$<" "$@"
