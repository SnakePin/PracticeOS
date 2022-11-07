#include "memory/memory.h"
#include "memory/internal.h"
#include "int_gate16/gate.h"
#include "memory/paging.h"
#include "utils.h"

// void memory_virt_reserve(void* pointer, size_t size);

void memory_virt_init()
{
    // TODO
}

void memory_virt_free(void *ptr, size_t size)
{
    memory_phy_free((physical_ptr_t)ptr, size);
    paging_unmap(ptr, size);
    paging_update_table();
}

void *memory_virt_allocate(size_t size)
{
    void *ptr = (void*)memory_phy_allocate_aligned(size);
    if (ptr == PHY_NULL)
    {
        return NULL;
    }
    paging_map((physical_ptr_t)ptr, (void *)ptr, size); // identity map
    paging_update_table();
    return (void *)ptr;
}

/*
MemoryVirtAllocLink_t memory_VirtAllocListRoot;

void* memory_virt_allocate(size_t size)
{
    // The allocated memory will contain the allocation link too
    size += sizeof(MemoryVirtAllocLink_t);

    void* firstfit = virt_get_firstfit(size);
    if (firstfit == NULL)
    {
        return PHY_NULL;
    }

    // Create the allocation link
    MemoryVirtAllocLink_t *allocLink = (MemoryVirtAllocLink_t *)firstfit;
    allocLink->BaseAddress = (void*)allocLink;
    allocLink->Length = size;
    allocLink->Prev = &memory_VirtAllocListRoot;
    allocLink->Next = memory_VirtAllocListRoot.Next;
    memory_VirtAllocListRoot.Next = allocLink;

    // Link next to us
    if (allocLink->Next != NULL)
    {
        ((MemoryVirtAllocLink_t *)allocLink->Next)->Prev = allocLink;
    }

    // Skip the allocation link when returning the pointer
    return firstfit + sizeof(MemoryVirtAllocLink_t);
}

void memory_virt_free(void* pointer)
{
    if (pointer == NULL)
    {
        // Can't free NULL pointer
        return;
    }

    // We don't have to traverse the linked list, the entry should be right under the pointer
    MemoryVirtAllocLink_t *target = ((MemoryVirtAllocLink_t *)pointer) - 1;
    void* actualDataPointer = target->BaseAddress + sizeof(MemoryVirtAllocLink_t);
    if (actualDataPointer != pointer)
    {
        // Trying to free invalid pointer?
        return;
    }

    if (target->Next != NULL)
    {
        // Connect next to previous
        ((MemoryVirtAllocLink_t *)target->Next)->Prev = target->Prev;
    }

    // Previous link always exists since there's a root
    ((MemoryVirtAllocLink_t *)target->Prev)->Next = target->Next;
}

void memory_virt_init()
{
    memory_VirtAllocListRoot.Prev = NULL;
    memory_VirtAllocListRoot.Next = NULL;
}
*/
