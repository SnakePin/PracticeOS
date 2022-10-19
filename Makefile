.PHONY: all clean kernel bootloader diskimg
all: kernel bootloader diskimg

diskimg: kernel bootloader
	dotnet-script mkdisk.csx

bootloader:
	@$(MAKE) -j$(shell nproc) -C src/bootloader

kernel:
	@$(MAKE) -j$(shell nproc) -C src/kernel

clean:
	@$(MAKE) -C src/bootloader clean
	@$(MAKE) -C src/kernel clean
	rm -rf out/*
