OUTPREFIX	= ../../out/bootloader
AS			= nasm # nasm my beloved, GAS requires section tricks to generate 16bit code
LD			= ld
ASFLAGS		= -f elf -F stabs -g
LDFLAGS		= 
ASM_OBJS    = utils.o mbr_code.o vbr_loader.o vbr_stage2.o vbr_kernel_trampoline.o

$(info $(shell mkdir -p $(OUTPREFIX)))

all: --build convert_elf
--build: vbr.elf mbr.elf

$(ASM_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) "$<" -o "$(OUTPREFIX)/$@"

vbr.elf: vbr_loader.o vbr_stage2.o utils.o vbr_kernel_trampoline.o
	$(LD) $(LDFLAGS) -T vbr_linker.ld -o "$(OUTPREFIX)/$@" $(addprefix $(OUTPREFIX)/, $^)

mbr.elf: mbr_code.o utils.o
	$(LD) $(LDFLAGS) -T mbr_linker.ld -o "$(OUTPREFIX)/$@" $(addprefix $(OUTPREFIX)/, $^)

convert_elf: --build
	objcopy -S -O binary "$(OUTPREFIX)/vbr.elf" "$(OUTPREFIX)/vbr_raw.bin" 
	objcopy -S -O binary "$(OUTPREFIX)/mbr.elf" "$(OUTPREFIX)/mbr_raw.bin" 

clean:
	rm -f $(OUTPREFIX)/*
