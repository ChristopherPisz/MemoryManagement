#pragma once

// Standard Includes
#include <cstdint>

//------------------------------------------------------------------------------
// Memory must be deallocated in inverse order it was allocated!
// So if you allocate object A and then object B you must free object B memory before you can free object A memory.
class StackMemoryManager
{
protected:

    struct AllocationHeader
    {
        void *  m_previousAddress;
        uint8_t m_adjustment;
    };

    size_t m_size;              // Total size of allocated memory, in bytes
    void * m_start;             // First address in allocated memory
    size_t m_usedMemory;        // Number of bytes used
    size_t m_numAllocations;    // Number of caller allocations that have occured
    void * m_previousPosition; 
    void * m_currentPosition;

public:

    StackMemoryManager(size_t size);
    StackMemoryManager(const StackMemoryManager &) = delete;
    StackMemoryManager & operator = (const StackMemoryManager &) = delete;
    ~StackMemoryManager();

    void * allocate(size_t size, uint8_t alignment);
    void free(void * p);
};
