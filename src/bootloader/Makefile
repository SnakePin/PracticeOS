OUTPREFIX	= ../../out/bootloader
AS			= nasm
LD			= ld
ASFLAGS		= -f elf -g -F stabs
LDFLAGS		= -s

all: ensure_outprefix --build convert_elf
--build: vbr.elf mbr.elf

mbr_code.o: mbr_code.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

vbr_code.o: vbr_code.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

utils.o: utils.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

vbr.elf: vbr_code.o utils.o
	$(LD) $(LDFLAGS) -T vbr_linker.ld -o "$(OUTPREFIX)/$@" "$(OUTPREFIX)/vbr_code.o" "$(OUTPREFIX)/utils.o"

mbr.elf: mbr_code.o utils.o
	$(LD) $(LDFLAGS) -T mbr_linker.ld -o "$(OUTPREFIX)/$@" "$(OUTPREFIX)/mbr_code.o" "$(OUTPREFIX)/utils.o"

convert_elf: --build
	objcopy -O binary "$(OUTPREFIX)/vbr.elf" "$(OUTPREFIX)/vbr_raw.bin" 
	objcopy -O binary "$(OUTPREFIX)/mbr.elf" "$(OUTPREFIX)/mbr_raw.bin" 

ensure_outprefix:
	mkdir -p "$(OUTPREFIX)"

clean:
	rm -f $(OUTPREFIX)/*
