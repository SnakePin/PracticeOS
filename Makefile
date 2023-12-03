.PHONY: all clean kernel bootloader diskimg shared
all: kernel bootloader diskimg

diskimg: kernel kernel_loader bootloader
	python3 mkdisk.py

bootloader:
	@$(MAKE) -j$(shell nproc) -C src/bootloader

kernel_loader: shared
	@$(MAKE) -j$(shell nproc) -C src/kernel_loader

kernel: shared
	@$(MAKE) -j$(shell nproc) -C src/kernel

shared:
	@$(MAKE) -j$(shell nproc) -C src/shared

clean:
	@$(MAKE) -C src/bootloader clean
	@$(MAKE) -C src/kernel clean
	@$(MAKE) -C src/shared clean
	@rm -rf out/* || true
