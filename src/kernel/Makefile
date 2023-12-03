OUT			= ../../out/kernel
SRC			= .
AS			= nasm
LD			= ld
CC			= gcc
CFLAGS		= -m32 -ffreestanding -nostdlib -c -std=c11 -pedantic -Wall -gdwarf-2
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align -L../../out/shared/ -l:libshared.a
INCLUDE		= $(SRC)/include ../shared/include

ASM_OBJS	= entry.o interrupt_asm.o ioport.o int_gate16/gate_kernel_code_asm.o
C_OBJS		= main.o interrupt_c.o drivers/vga.o drivers/pic8259.o int_gate16/gate_kernel_code_c.o memory/memory_virt.o memory/memory_phy.o
ASM_OBJ_TARGETS = $(addprefix $(OUT)/, $(ASM_OBJS))
C_OBJ_TARGETS = $(addprefix $(OUT)/, $(C_OBJS))

.PHONY: all clean ensure_path
all: $(OUT)/kernel_min.elf $(OUT)/kernel.elf

ensure_path:
	@mkdir -p $(OUT)
	@mkdir -p $(OUT)/drivers
	@mkdir -p $(OUT)/int_gate16
	@mkdir -p $(OUT)/memory

clean:
	@rm -rf $(OUT)/* || true

$(ASM_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.s | ensure_path
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(C_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.c | ensure_path
	$(CC) $(CFLAGS) "$<" -o "$@" $(INCLUDE:%=-I%)

$(OUT)/kernel.elf: $(ASM_OBJ_TARGETS) $(C_OBJ_TARGETS) $(OUT)/int_gate16/gate16_embed.o | ensure_path
	$(LD) -T kernel_linker.ld -o "$@" $^ $(LDFLAGS)

$(OUT)/int_gate16/gate16_raw.bin: $(SRC)/int_gate16/gate16_raw.s | ensure_path
	$(AS) -f bin "$<" -o "$@"

$(OUT)/int_gate16/gate16_embed.o: $(SRC)/int_gate16/gate16_embed.s $(OUT)/int_gate16/gate16_raw.bin | ensure_path
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(OUT)/kernel_min.elf: $(OUT)/kernel.elf | ensure_path
	strip -s "$<" -o "$@"
