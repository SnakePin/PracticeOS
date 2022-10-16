#!/usr/bin/env dotnet-script
//dotnet-script runs scripts with C# 9.0 Top Level Statements enabled
using System;
using System.Runtime.InteropServices;

enum MBRPartitionTypes : byte
{
    Free = 0x00,
    FAT32wCHS = 0x0B,
    FAT32wLBA = 0x0C
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

static byte[] UInt16ToLEBytes(UInt16 value) {
    byte[] bytes = BitConverter.GetBytes(value);
    if (!BitConverter.IsLittleEndian) {
        Array.Reverse(bytes);
    }
    return bytes;
}

static void WriteMBRPTE(Stream strm, bool active, MBRPartitionTypes type, uint lbaStart, uint sectorCount)
{
    var pte = new MBRPTEStruct();
    //I really do hate CHS
    pte.CHSFirst = new byte[3] { 0, 0, 0 };
    pte.CHSLast = new byte[3] { 0, 0, 0 };
    pte.LBAFirst = lbaStart;
    pte.SectorCount = sectorCount;
    pte.Status = (byte)(active ? 0x80 : 0x00);
    pte.Type = (byte)type;
    strm.Write(GetBytesFromStruct(pte));
}

const string diskOutFile = "out/disk.img";
const string mbrBootstrapPath = "out/bootloader/mbr_raw.bin";
const string vbrFilePath = "out/bootloader/vbr_raw.bin";
const string kernelFilePath = "out/kernel/kernel.bin";
const int MBRBootstrapEnd = 446;
const int VBRMax = 522495; //522495 is the length of continuous memory right after the the MBR in RAM

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
    WriteMBRPTE(fs, true, MBRPartitionTypes.FAT32wLBA, partitionBaseLBA, (uint)partitionSize/LBASectorSize);
    WriteMBRPTE(fs, false, MBRPartitionTypes.Free, 0, 0);
    WriteMBRPTE(fs, false, MBRPartitionTypes.Free, 0, 0);
    WriteMBRPTE(fs, false, MBRPartitionTypes.Free, 0, 0);
    fs.Write(new byte[] { 0x55, 0xAA });

    //VBR
    fs.Seek(partitionBase, SeekOrigin.Begin);
    fs.Write(vbrBytes);

    //Kernel, TODO: Make this a file instead
    fs.Seek(partitionBase+0xFFFE, SeekOrigin.Begin);
    //This calculation is only valid if the kernel is aligned to LBASectorSize boundary
    UInt16 kernelSectorSpan = (ushort)Math.Ceiling((double)kernelBytes.Length / LBASectorSize);
    fs.Write(UInt16ToLEBytes(kernelSectorSpan));
    fs.Write(kernelBytes); //0x10000

    //Partition 1's last byte
    fs.Seek(partitionBase+partitionSize-1, SeekOrigin.Begin);
    fs.WriteByte(0x00);
}
Console.WriteLine("Disk image written...");
