

// Project Includes
#include "LinearMemoryManager.h"

// Common Library Includes
#include "Exception.h"

//  Standard Includes
#include <memory>

//------------------------------------------------------------------------------
LinearMemoryManager::LinearMemoryManager(size_t size)
    :
    m_size(size)
  , m_usedMemory(0)
{
    if (size <= 0)
    {
        // Error - Invalid size
        const std::string msg("Invalid size requested. Size must be greater than zero.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_start = ::malloc(size);

    if (!m_start)
    {
        // Error - System failed to allocate requested size
        const std::string msg("System failed to allocate requested size");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_currentPosition = m_start;
}

//------------------------------------------------------------------------------
LinearMemoryManager::~LinearMemoryManager()
{
    ::free(m_start);
}

//------------------------------------------------------------------------------
void * LinearMemoryManager::allocate(size_t size, uint8_t alignment)
{
    if (size <= 0)
    {
        // Error - Invalid size
        const std::string msg("Invalid size requested. Size must be greater than zero.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if (!alignment)
    {
        // Error - Invalid alignment
        const std::string msg("Invalid alignment. Alignment must be greater than zero.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    size_t availableSpace = m_size - m_usedMemory;
    void * alignedAddress = std::align(alignment, size, m_currentPosition, availableSpace);

    if( !alignedAddress )
    {
        // Error - Could not fit the desired number of bytes aligned by the specified alignment into the available memory space
        const std::string msg(" Could not fit the desired number of bytes aligned by the specified alignment into the available memory space.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Calculate the new start of the free memory
    const size_t adjustment = static_cast<uint8_t *>(alignedAddress) - static_cast<uint8_t *>(m_currentPosition);
    
    m_usedMemory += adjustment + size;
    m_currentPosition = (static_cast<uint8_t *>(alignedAddress) + size);
    ++m_numAllocations;

    return alignedAddress;
}

//------------------------------------------------------------------------------
void LinearMemoryManager::free(void * p)
{
    // Error - This class does not support freeing by pointer. It only frees the entire pool at once via clear()
    const std::string msg("LinearMemoryManager does not support freeing by pointer. It only frees the entire pool at once via clear()");
    throw Common::Exception(__FILE__, __LINE__, msg);
}

//------------------------------------------------------------------------------
void LinearMemoryManager::clear()
{
    m_numAllocations  = 0;
    m_usedMemory      = 0;
    m_currentPosition = m_start;
}
