OUT			= ../../out/shared
AS			= nasm
AR			= ar
CC			= gcc
CXX			= g++

INCLUDE		= include
CFLAGS		= -c -m32 -ffreestanding -nostdlib -std=c11 -pedantic -Wall -gdwarf-2 $(INCLUDE:%=-I%)
CPPFLAGS	= -c -m32 -ffreestanding -nostdlib -std=c++17 -pedantic -Wall -gdwarf-2 $(INCLUDE:%=-I%)
ASFLAGS		= -f elf32 -gdwarf
ARFLAGS		= -crs

SOURCES := $(wildcard *.s *.c *.cpp)
OBJS := $(patsubst %,$(OUT)/%.o,$(SOURCES))

# Phony targets
.PHONY: all clean
all: $(OUT)/libshared.a
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

$(OUT)/libshared.a: $(OBJS)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) "$@" $^
