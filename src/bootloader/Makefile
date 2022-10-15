OUTPREFIX	= ../../out/bootloader
AS			= nasm # nasm my beloved, GAS requires section tricks to generate 16bit code
LD			= ld
ASFLAGS		= -f elf -g -F stabs
LDFLAGS		= 
ASM_OBJS    = utils.o mbr_code.o vbr_loader.o vbr_stage2.o

all: ensure_outprefix --build convert_elf
--build: vbr.elf mbr.elf

$(ASM_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

vbr.elf: vbr_loader.o vbr_stage2.o utils.o
	$(LD) $(LDFLAGS) -T vbr_linker.ld -o "$(OUTPREFIX)/$@" $(addprefix $(OUTPREFIX)/, $^)

mbr.elf: mbr_code.o utils.o
	$(LD) $(LDFLAGS) -T mbr_linker.ld -o "$(OUTPREFIX)/$@" $(addprefix $(OUTPREFIX)/, $^)

convert_elf: --build
	objcopy -O binary "$(OUTPREFIX)/vbr.elf" "$(OUTPREFIX)/vbr_raw.bin" 
	objcopy -O binary "$(OUTPREFIX)/mbr.elf" "$(OUTPREFIX)/mbr_raw.bin" 

ensure_outprefix:
	mkdir -p "$(OUTPREFIX)"

clean:
	rm -f $(OUTPREFIX)/*
