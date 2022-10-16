all: --build

--build: kernel bootloader diskimg

diskimg: kernel bootloader
	dotnet-script mkdisk.csx

bootloader:
	$(MAKE) -j$(nproc) -C src/bootloader

kernel:
	mkdir -p out/kernel
	touch out/kernel/kernel.bin
#	$(MAKE) -j$(nproc) -C src/kernel

clean:
	$(MAKE) -C src/bootloader clean
	rm -rf out/*