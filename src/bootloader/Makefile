OUT			= ../../out/bootloader
SRC			= .
AS			= nasm # nasm my beloved, GAS requires section tricks to generate 16bit code
LD			= ld
ASFLAGS		= -f elf32 -gdwarf
LDFLAGS		= --warn-section-align

ASM_OBJS    = utils.o mbr_main.o vbr_main.o vbr_trampoline.o
ASM_OBJ_TARGETS = $(addprefix $(OUT)/, $(ASM_OBJS))
OBJCOPY_TARGETS = $(addprefix $(OUT)/, vbr.bin mbr.bin)

.PHONY: all clean ensure_path
all: $(OUT)/vbr.bin $(OUT)/mbr.bin

ensure_path:
	@mkdir -p $(OUT)

$(ASM_OBJ_TARGETS): $(OUT)/%.o: $(SRC)/%.s | ensure_path
	$(AS) $(ASFLAGS) "$<" -o "$@"

$(OUT)/vbr.elf: $(addprefix $(OUT)/, vbr_main.o vbr_trampoline.o utils.o)
	$(LD) $(LDFLAGS) -T vbr_linker.ld -o "$(OUT)/$@" $(addprefix $(OUT)/, $^)

$(OUT)/mbr.elf: $(addprefix $(OUT)/, mbr_main.o utils.o)
	$(LD) $(LDFLAGS) -T mbr_linker.ld -o "$(OUT)/$@" $(addprefix $(OUT)/, $^)

$(OBJCOPY_TARGETS): $(OUT)/%.bin: $(OUT)/%.elf
	objcopy -S -O binary "$<" "$@"

clean:
	@rm -rf $(OUT)/* || true
