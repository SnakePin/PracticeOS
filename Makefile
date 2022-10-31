.PHONY: all clean kernel bootloader diskimg
all: kernel bootloader diskimg

diskimg: kernel kernel_loader bootloader
	dotnet-script mkdisk.csx

bootloader:
	@$(MAKE) -j$(shell nproc) -C src/bootloader

kernel_loader:
	@$(MAKE) -j$(shell nproc) -C src/kernel_loader

kernel:
	@$(MAKE) -j$(shell nproc) -C src/kernel

clean:
	@$(MAKE) -C src/bootloader clean
	@$(MAKE) -C src/kernel clean
	@rm -rf out/* || true
