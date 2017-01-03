#pragma once

// Standard Includes
#include <cstdint>

//------------------------------------------------------------------------------
class FreeListMemoryManager
{
protected:

    struct AllocationHeader
    {
        size_t  m_size;
        uint8_t m_adjustment;
    };

    struct FreeBlock
    {
        size_t      m_size;
        FreeBlock * m_next;
    };

    size_t      m_size;            // Total size of allocated memory, in bytes
    void *      m_start;           // First address in allocated memory
    size_t      m_usedMemory;      // Number of bytes used
    size_t      m_numAllocations;  // Number of caller allocations that have occured
    FreeBlock * m_freeBlocks;

public:

    FreeListMemoryManager(size_t size);
    FreeListMemoryManager(const FreeListMemoryManager &) = delete;
    FreeListMemoryManager & operator = (const FreeListMemoryManager &) = delete;
    ~FreeListMemoryManager();

    void * allocate(size_t size, uint8_t alignment);
    void free(void* p);
};
