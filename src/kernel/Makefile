OUT			= ../../out/kernel
AS			= nasm
LD			= ld
CC			= gcc
CXX			= g++

INCLUDE		= include ../shared/include
CFLAGS		= -c -m32 -ffreestanding -nostdlib -std=c11 -pedantic -Wall -gdwarf-2 $(INCLUDE:%=-I%)
CPPFLAGS	= -c -m32 -ffreestanding -nostdlib -std=c++17 -pedantic -Wall -gdwarf-2 $(INCLUDE:%=-I%)
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align -L../../out/shared/ -l:libshared.a

SOURCES := $(wildcard *.s drivers/*.s memory/*.s *.c drivers/*.c memory/*.c *.cpp drivers/*.cpp memory/*.cpp) \
			int_gate16/gate_kernel_code_c.c int_gate16/gate_kernel_code_asm.s int_gate16/gate16_embed.s
OBJS := $(patsubst %,$(OUT)/%.o,$(SOURCES))

# Phony targets
.PHONY: all clean
all: $(OUT)/kernel_min.elf $(OUT)/kernel.elf
clean:
	@rm -rf $(OUT)/* || true

$(OUT)/int_gate16/gate16_embed.s.o: int_gate16/gate16_embed.s int_gate16/gate16_raw.s
	@mkdir -p $(@D)
	$(AS) -f bin "int_gate16/gate16_raw.s" -o "$(OUT)/int_gate16/gate16_raw.bin"
	$(AS) $(ASFLAGS) "int_gate16/gate16_embed.s" -o "$@"

$(OUT)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) "$<" -o "$@"

$(OUT)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) "$<" -o "$@"

$(OUT)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(OUT)/kernel.elf: $(OBJS)
	@mkdir -p $(@D)
	$(LD) -T kernel_linker.ld -o "$@" $^ $(LDFLAGS)

$(OUT)/kernel_min.elf: $(OUT)/kernel.elf
	@mkdir -p $(@D)
	strip -s "$<" -o "$@"
