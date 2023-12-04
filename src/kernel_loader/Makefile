OUT			= ../../out/kernel_loader
AS			= nasm
LD			= ld
CC			= gcc
CXX			= g++

INCLUDE		= include ../shared/include
CFLAGS		= -c -m32 -ffreestanding -nostdlib -std=c11 -pedantic -Wall -gdwarf-2 $(INCLUDE:%=-I%)
CPPFLAGS	= -c -m32 -ffreestanding -nostdlib -std=c++17 -pedantic -Wall -gdwarf-2 $(INCLUDE:%=-I%)
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align -L../../out/shared/ -l:libshared.a

SOURCES := $(wildcard *.s *.c *.cpp simple_gate16/*.s simple_gate16/*.c simple_gate16/*.cpp)
OBJS := $(patsubst %,$(OUT)/%.o,$(SOURCES))

# Phony targets
.PHONY: all clean
all: $(OUT)/kernel_loader.bin
clean:
	@rm -rf $(OUT)/* || true

$(OUT)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) "$<" -o "$@"

$(OUT)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) "$<" -o "$@"

$(OUT)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(OUT)/kernel_loader.elf: $(OBJS)
	@mkdir -p $(@D)
	$(LD) -T kernel_ldr_linker.ld -o "$@" $^ $(LDFLAGS)

# Should write an ELF loader in the BL instead.
$(OUT)/%.bin: $(OUT)/%.elf
	@mkdir -p $(@D)
	objcopy --set-section-flags .bss=alloc,load,contents -S -O binary "$<" "$@"
