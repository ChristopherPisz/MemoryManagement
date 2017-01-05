#pragma once

// Standard Includes
#include <cstdint>

//------------------------------------------------------------------------------
// A pool of memory that us allocated once and freed once
//
// New allocations simply move the pointer to the first free address forward.
// Individual deallocations cannot be made, instead use clear() to completely clear the memory used by the allocator.
class LinearMemoryManager
{
protected:

    size_t m_size;             // Total size of allocated memory, in bytes
    void * m_start;            // First address in allocated memory
    size_t m_usedMemory;       // Number of bytes used
    size_t m_numAllocations;   // Number of caller allocations that have occured
    void * m_currentPosition;  // Address to first available free byte

public:

    LinearMemoryManager(size_t size);
    LinearMemoryManager(const LinearMemoryManager &) = delete;
    LinearMemoryManager & operator = (const LinearMemoryManager &) = delete;
    ~LinearMemoryManager();

    void * allocate(size_t size, uint8_t alignment);
    void clear();
};

