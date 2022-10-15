
all: --build

--build: bootloader diskimg

diskimg:
	dotnet-script mkdisk.csx

bootloader:
	$(MAKE) -C src/bootloader

clean:
	$(MAKE) -C src/bootloader clean
	rm -rf out/*