#pragma once

// Standard Includes
#include <cstdint>

//------------------------------------------------------------------------------
class IMemoryManager
{
public:

    IMemoryManager() {};
    virtual ~IMemoryManager() {};

    virtual void * allocate(size_t size, uint8_t alignment) = 0;
    virtual void free(void * p) = 0;
};

//------------------------------------------------------------------------------
