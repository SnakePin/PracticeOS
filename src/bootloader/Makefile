OUT			= ../../out/bootloader
SRC			= .
AS			= nasm # nasm my beloved, GAS requires section tricks to generate 16bit code
LD			= ld
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align

VBR_OBJS := $(patsubst %,$(OUT)/%.o, vbr_main.s vbr_trampoline.s utils.s)
MBR_OBJS := $(patsubst %,$(OUT)/%.o, mbr_main.s utils.s)

# Phony targets
.PHONY: all clean
all: $(OUT)/vbr.bin $(OUT)/mbr.bin
clean:
	@rm -rf $(OUT)/* || true

$(OUT)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(OUT)/%.bin: $(OUT)/%.elf
	@mkdir -p $(@D)
	objcopy --set-section-flags .bss=alloc,load,contents -S -O binary "$<" "$@"

$(OUT)/vbr.elf: $(VBR_OBJS)
	@mkdir -p $(@D)
	$(LD) -T vbr_linker.ld -o "$(OUT)/$@" $(addprefix $(OUT)/, $^) $(LDFLAGS)

$(OUT)/mbr.elf: $(MBR_OBJS)
	@mkdir -p $(@D)
	$(LD) -T mbr_linker.ld -o "$(OUT)/$@" $(addprefix $(OUT)/, $^) $(LDFLAGS)
