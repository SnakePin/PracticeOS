#!/usr/bin/env dotnet-script
//dotnet-script runs scripts with C# 9.0 Top Level Statements enabled
using System;
using System.Runtime.InteropServices;

enum MBRPartitionTypes : byte
{
    Free = 0x00,
    FAT32wCHS = 0x0B,
    FAT32wLBA = 0x0C,
    LinuxNativeFS = 0x83
};

[StructLayout(LayoutKind.Sequential)]
public struct MBRPTEStruct
{
    [MarshalAs(UnmanagedType.U1)]
    public byte Status;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public byte[] CHSFirst;
    [MarshalAs(UnmanagedType.U1)]
    public byte Type;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public byte[] CHSLast;
    [MarshalAs(UnmanagedType.U4)]
    public UInt32 LBAFirst;
    [MarshalAs(UnmanagedType.U4)]
    public UInt32 SectorCount;
}

const uint LBASectorSize = 512;
static uint LBAToByteAddress(uint addr)
{
    return addr * LBASectorSize;
}

static byte[] GetBytesFromStruct<T>(T str) where T : struct
{
    int size = Marshal.SizeOf(str);
    byte[] arr = new byte[size];
    GCHandle h = default(GCHandle);
    try
    {
        h = GCHandle.Alloc(arr, GCHandleType.Pinned);
        Marshal.StructureToPtr<T>(str, h.AddrOfPinnedObject(), false);
    }
    finally
    {
        if (h.IsAllocated)
        {
            h.Free();
        }
    }
    return arr;
}

static byte[] MakeCHS(int cylinder, int head, int sector) {
    byte cylinderHighBits = (byte)((cylinder&0x0300)>>8);
    return new byte[3] { (byte)head, (byte)((sector&0x3F)|cylinderHighBits), (byte)cylinder };
}

static void WriteMBRPTE(Stream strm, bool active, MBRPartitionTypes type, uint lbaStart, uint sectorCount)
{
    var pte = new MBRPTEStruct();
    pte.CHSFirst = MakeCHS(1023, 254, 63); //Invalid CHS value
    pte.CHSLast = MakeCHS(1023, 254, 63); //Invalid CHS value
    pte.LBAFirst = lbaStart;
    pte.SectorCount = sectorCount;
    pte.Status = (byte)(active ? 0x80 : 0x00);
    pte.Type = (byte)type;
    strm.Write(GetBytesFromStruct(pte));
}

const string diskOutFile = "out/disk.img";
const string mbrBootstrapPath = "out/bootloader/mbr.bin";
const string vbrFilePath = "out/bootloader/vbr.bin";
const string kernelLdrFilePath = "out/kernel_loader/kernel_loader.bin";
const string kernelFilePath = "out/kernel/kernel_min.elf";
const int MBRBootstrapEnd = 446;
const int VBRMax = 512;

//Main code starts here
var mbrBootstrapBytes = File.ReadAllBytes(mbrBootstrapPath);
if (mbrBootstrapBytes.Length > MBRBootstrapEnd)
{
    Console.WriteLine($"MBR Bootstrap code is larger than {MBRBootstrapEnd} bytes");
    return 1;
}
var vbrBytes = File.ReadAllBytes(vbrFilePath);
if (vbrBytes.Length > VBRMax)
{
    Console.WriteLine($"VBR code must be less than {VBRMax} bytes.");
    return 1;
}
var kernelLdrBytes = File.ReadAllBytes(kernelLdrFilePath);
var kernelBytes = File.ReadAllBytes(kernelFilePath);

Console.WriteLine("Writing disk image...");
using (FileStream fs = File.Open(diskOutFile, FileMode.Create, FileAccess.Write, FileShare.None))
{
    uint partitionBaseLBA = 64;
    uint partitionBase = LBAToByteAddress(partitionBaseLBA);
    uint partitionSize = (uint)Math.Pow(2, 22); //4MiB

    fs.Write(mbrBootstrapBytes);
    fs.Seek(MBRBootstrapEnd, SeekOrigin.Begin);
    //Active, starts at LBA 64, 16MiB
    WriteMBRPTE(fs, true, MBRPartitionTypes.LinuxNativeFS, partitionBaseLBA, (uint)partitionSize/LBASectorSize);
    WriteMBRPTE(fs, false, MBRPartitionTypes.Free, 0, 0);
    WriteMBRPTE(fs, false, MBRPartitionTypes.Free, 0, 0);
    WriteMBRPTE(fs, false, MBRPartitionTypes.Free, 0, 0);
    fs.Write(new byte[] { 0x55, 0xAA });

    //VBR
    fs.Seek(partitionBase, SeekOrigin.Begin);
    fs.Write(vbrBytes);

    //Kernel loader, TODO: Make this a file instead
    fs.Seek(partitionBase+(0x10000-2), SeekOrigin.Begin);
    fs.Write(BitConverter.GetBytes((UInt16)Math.Ceiling((double)kernelLdrBytes.Length / LBASectorSize)));
    fs.Write(kernelLdrBytes); //0x10000

    //Kernel, TODO: Make this a file instead
    fs.Seek(partitionBase+(0x20000-4), SeekOrigin.Begin);
    fs.Write(BitConverter.GetBytes((UInt32)kernelBytes.Length)); //Length is in bytes for the kernel image only
    fs.Write(kernelBytes); //0x20000

    //Partition 1's last byte
    fs.Seek(partitionBase+partitionSize-1, SeekOrigin.Begin);
    fs.WriteByte(0x00);
}
Console.WriteLine("Disk image written...");
