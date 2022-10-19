OUT	= ../../out/bootloader
SRC = .
AS			= nasm # nasm my beloved, GAS requires section tricks to generate 16bit code
LD			= ld
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align

ASM_OBJS    = utils.o mbr_code.o vbr_loader.o vbr_stage2.o vbr_stage2_trampoline.o
ASM_OBJ_TARGETS = $(addprefix $(OUT)/, $(ASM_OBJS))
OBJCOPY_TARGETS = $(addprefix $(OUT)/, vbr.bin mbr.bin)

$(info $(shell mkdir -p $(OUT)))

.PHONY: all clean
all: $(OUT)/vbr.bin $(OUT)/mbr.bin

$(ASM_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.s
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(OUT)/vbr.elf: $(addprefix $(OUT)/, vbr_loader.o vbr_stage2.o utils.o vbr_stage2_trampoline.o)
	$(LD) $(LDFLAGS) -T vbr_linker.ld -o "$(OUT)/$@" $(addprefix $(OUT)/, $^)

$(OUT)/mbr.elf: $(addprefix $(OUT)/, mbr_code.o utils.o)
	$(LD) $(LDFLAGS) -T mbr_linker.ld -o "$(OUT)/$@" $(addprefix $(OUT)/, $^)

$(OBJCOPY_TARGETS): $(OUT)/%.bin: $(OUT)/%.elf
	objcopy -S -O binary "$<" "$@"

clean:
	rm -f $(OUT)/*
