OUT			= ../../out/shared
SRC			= .
AS			= nasm
AR			= ar
CC			= gcc
CFLAGS		= -m32 -ffreestanding -nostdlib -c -std=c11 -pedantic -Wall -gdwarf-2
ASFLAGS		= -f elf32 -gdwarf
INCLUDE		= $(SRC)/include

ASM_OBJS	= gdt_asm.o paging_asm.o
C_OBJS		= utils.o gdt_c.o paging_c.o memory.o
ASM_OBJ_TARGETS = $(addprefix $(OUT)/, $(ASM_OBJS))
C_OBJ_TARGETS = $(addprefix $(OUT)/, $(C_OBJS))

.PHONY: all clean ensure_path
all: $(OUT)/libshared.a

ensure_path:
	@mkdir -p $(OUT)

clean:
	@rm -rf $(OUT)/* || true

$(ASM_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.s | ensure_path
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(C_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.c | ensure_path
	$(CC) $(CFLAGS) "$<" -o "$@" $(INCLUDE:%=-I%)

$(OUT)/libshared.a: $(ASM_OBJ_TARGETS) $(C_OBJ_TARGETS)
	$(AR) -crs "$@" $^
