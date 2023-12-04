.PHONY: all clean kernel bootloader diskimg shared
all: kernel bootloader diskimg

diskimg: kernel kernel_loader bootloader
	python3 mkdisk.py

bootloader:
	@$(MAKE) -j$(shell nproc) -C src/bootloader all

kernel_loader: shared
	@$(MAKE) -j$(shell nproc) -C src/kernel_loader all

kernel: shared
	@$(MAKE) -j$(shell nproc) -C src/kernel all

shared:
	@$(MAKE) -j$(shell nproc) -C src/shared all

clean:
	@$(MAKE) -C src/bootloader clean
	@$(MAKE) -C src/kernel clean
	@$(MAKE) -C src/kernel_loader clean
	@$(MAKE) -C src/shared clean
	@rm -rf out/* || true
