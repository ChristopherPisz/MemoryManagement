#pragma once

// Standard Includes
#include <cstdint>

//------------------------------------------------------------------------------
/// <summary>
/// Base class for memory managers that are not fixed size
/// </summary>
class IMemoryManager
{
public:

    IMemoryManager() {};
    virtual ~IMemoryManager() {};

    virtual void * allocate(size_t size, uint8_t alignment) = 0;
    virtual void free(void * p) = 0;
};

//------------------------------------------------------------------------------
