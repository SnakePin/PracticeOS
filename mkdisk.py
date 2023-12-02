import struct
from enum import Enum

# Even if the disk is 4K-native, BIOS usually treats it as 512
LBASectorSize = 512
MBR_max = 446
VBR_max = 512


def MiB(val):
    MiB_in_bytes = 0x100000
    return val * MiB_in_bytes


def LBA_to_byte_address(lba):
    return lba * LBASectorSize


def byte_size_to_sector_count(val):
    return -(val // -LBASectorSize)


def make_chs(cylinder, head, sector):
    cylinder_high_bits = (cylinder & 0x0300) >> 8
    return bytes([head, (sector & 0x3F) | cylinder_high_bits, cylinder & 0xFF])


class PartitionTypes(Enum):
    Free = 0x00
    FAT32wCHS = 0x0B
    FAT32wLBA = 0x0C
    LinuxNativeFS = 0x83


class PartitionTableEntry:
    def __init__(
        self,
        status=0,
        pte_type=PartitionTypes.Free.value,
        lba_first=0,
        sector_count=0,
        chs_first=make_chs(1023, 254, 63),
        chs_last=make_chs(1023, 254, 63),
    ):
        self.status = status
        self.chs_first = chs_first
        self.type = pte_type
        self.chs_last = chs_last
        self.lba_first = lba_first
        self.sector_count = sector_count

    def pack(self):
        return struct.pack(
            "<B3sB3sII",
            self.status,
            bytes(self.chs_first),
            self.type,
            bytes(self.chs_last),
            self.lba_first,
            self.sector_count,
        )


class MBRStruct:
    def __init__(self, bootstrap_code, partitions, signature):
        self.bootstrap_code = bootstrap_code
        self.partitions = partitions
        self.signature = signature

    def pack(self):
        partitions_raw = b"".join(partition.pack() for partition in self.partitions)
        return struct.pack(
            "<446s64sH", self.bootstrap_code, partitions_raw, self.signature
        )


class StreamPartitionWriter:
    def __init__(self, file_stream, partition_base, partition_size):
        self.file_stream = file_stream
        self.partition_base = partition_base
        self.partition_size = partition_size
        self.zero_fill()

    def zero_fill(self):
        self.seek_and_write(0, bytearray(self.partition_size))

    def seek_and_write(self, offset, data):
        self.file_stream.seek(self.partition_base + offset)
        self.file_stream.write(data)

    def write_vbr(self, vbr_bytes):
        self.seek_and_write(0, vbr_bytes)

    def write_kernel_loader(self, kernel_ldr_bytes):
        # Kernel loader size is in sectors because it's easier for MBR bootstrap to parse
        # Should probably make these proper files
        length_in_sectors = byte_size_to_sector_count(len(kernel_ldr_bytes))
        self.seek_and_write(0x1000 - 2, struct.pack("<H", length_in_sectors))
        self.file_stream.write(kernel_ldr_bytes)

    def write_kernel(self, kernel_bytes):
        self.seek_and_write(0x4000 - 4, struct.pack("<I", len(kernel_bytes)))
        self.file_stream.write(kernel_bytes)


def main():
    mbr_bootstrap_bytes = bytes()
    vbr_bytes = bytes()
    kernel_loader_bytes = bytes()
    kernel_bytes = bytes()

    with open("out/bootloader/mbr.bin", "rb") as f:
        mbr_bootstrap_bytes = f.read()
        if len(mbr_bootstrap_bytes) > MBR_max:
            print(f"MBR Bootstrap code is larger than {MBR_max} bytes")
            return 1

    with open("out/bootloader/vbr.bin", "rb") as f:
        vbr_bytes = f.read()
        if len(vbr_bytes) > VBR_max:
            print(f"VBR code must be less than {VBR_max} bytes.")
            return 1

    with open("out/kernel_loader/kernel_loader.bin", "rb") as f:
        kernel_loader_bytes = f.read()

    with open("out/kernel/kernel_min.elf", "rb") as f:
        kernel_bytes = f.read()

    with open("out/disk.img", "wb") as f:
        partition_lba_base = 64
        partition_size = MiB(1)

        partitionList = [
            PartitionTableEntry(
                status=0x80,
                sector_count=byte_size_to_sector_count(partition_size),
                lba_first=partition_lba_base,
                pte_type=PartitionTypes.LinuxNativeFS.value,
            ),
            PartitionTableEntry(),
            PartitionTableEntry(),
            PartitionTableEntry(),
        ]
        
        f.write(
            MBRStruct(
                bootstrap_code=mbr_bootstrap_bytes,
                partitions=partitionList,
                signature=0xAA55,
            ).pack()
        )

        partWriter = StreamPartitionWriter(
            f, LBA_to_byte_address(partition_lba_base), partition_size
        )
        partWriter.write_vbr(vbr_bytes)
        partWriter.write_kernel_loader(kernel_loader_bytes)
        partWriter.write_kernel(kernel_bytes)


exit(main())
