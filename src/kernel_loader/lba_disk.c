#include <stdint.h>
#include "utils.h"
#include "int_gate16/gate.h"

#define EFLAGS_CF_MASK 0x1
#define LBA_SECTOR_SIZE 512
#define INT_GATE16_LBA_XFER_PKT INT_GATE16_FREE_MEM
#define INT_GATE16_LBA_BUFFER (INT_GATE16_LBA_XFER_PKT + sizeof(LBATransferPacket_t))

typedef struct
{
    uint8_t sizeOfPacket;
    uint8_t unused;
    uint16_t numSector;
    uint16_t memOffset;
    uint16_t memSegment;
    uint64_t lbaAddress;
} PACKED_ATTR LBATransferPacket_t;

enum LBARequestType
{
    LBARead = 0x42,
    LBAWrite = 0x43,
};

static uint8_t send_lba_request(uint8_t diskID, LBATransferPacket_t *packet, uint32_t lbaRequestType)
{
    memcpy((void *)INT_GATE16_LBA_XFER_PKT, packet, sizeof(LBATransferPacket_t));
    INTGate16Registers_t inRegs = {.edx = diskID, .eax = lbaRequestType<<8, .esi = INT_GATE16_LBA_XFER_PKT};
    INTGate16Registers_t outRegs;
    int_gate16_make_call(0x13, &inRegs, &outRegs);
    return !(outRegs.eflags_ro & EFLAGS_CF_MASK);
}

uint8_t lba_read_from_disk(uint8_t diskID, uint32_t lbaOffset, uint64_t byteOffset, size_t length, void *outBuffer)
{
    uintptr_t loadAddr = INT_GATE16_LBA_BUFFER;
    uint64_t firstLBAToRead = lbaOffset + (byteOffset / LBA_SECTOR_SIZE);
    size_t offsetInSector = byteOffset % LBA_SECTOR_SIZE;
    size_t sectorsToRead = ALIGN_ADDRESS_CEIL(offsetInSector + length, LBA_SECTOR_SIZE) / LBA_SECTOR_SIZE;

    LBATransferPacket_t packet = {.sizeOfPacket = sizeof(LBATransferPacket_t),
                                  .unused = 0,
                                  .numSector = sectorsToRead,
                                  .memSegment = loadAddr / 16,
                                  .memOffset = loadAddr % 16,
                                  .lbaAddress = firstLBAToRead};
    if (!send_lba_request(diskID, &packet, LBARead))
    {
        return 0;
    }
    memcpy(outBuffer, (void *)(loadAddr + offsetInSector), length);
    return 1;
}
