all: --build

--build: kernel bootloader diskimg

diskimg: kernel bootloader
	dotnet-script mkdisk.csx

bootloader:
	$(MAKE) -j$(nproc) -C src/bootloader

kernel:
	$(MAKE) -j$(nproc) -C src/kernel

clean:
	$(MAKE) -C src/bootloader clean
	$(MAKE) -C src/kernel clean
	rm -rf out/*