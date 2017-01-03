

// Project Includes
#include "StackMemoryManager.h"

// Common Libary
#include "Exception.h"

// Standard Includes
#include <string>

//------------------------------------------------------------------------------
StackMemoryManager::StackMemoryManager(size_t size)
    :
    m_size(size)
  , m_start(nullptr)
  , m_currentPosition(nullptr)
  , m_previousPosition(nullptr)
  , m_usedMemory(0)
  , m_numAllocations(0)
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
StackMemoryManager::~StackMemoryManager()
{
    ::free(m_start);
}

//------------------------------------------------------------------------------
void * StackMemoryManager::allocate(size_t size, uint8_t alignment)
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
    
    if (!alignedAddress)
    {
        // Error - Could not fit the desired number of bytes aligned by the specified alignment into the available memory space
        const std::string msg(" Could not fit the desired number of bytes aligned by the specified alignment into the available memory space.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if the space given by the alignment was enough to contain the header
    uint8_t adjustment  = static_cast<int8_t>(static_cast<uint8_t *>(alignedAddress) - static_cast<uint8_t *>(m_currentPosition));
    uint8_t neededSpace = sizeof(AllocationHeader);
    
    if (adjustment < neededSpace)
    {
        neededSpace -= adjustment;

        // Increase adjustment to fit header
        adjustment += alignment * (neededSpace / alignment);

        if( neededSpace % alignment > 0 )
        {
            adjustment += alignment;
        }
    }

    if (m_usedMemory + adjustment + size > m_size)
    {
        // Error - Could not fit the desired number of bytes aligned by the specified alignment into the available memory space, with header
        const std::string msg(" Could not fit the desired number of bytes aligned by the specified alignment into the available memory space, with header");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    alignedAddress = static_cast<uint8_t *>(m_currentPosition) + adjustment;

    // Add Allocation Header
    AllocationHeader * header = reinterpret_cast<AllocationHeader *>(static_cast<uint8_t *>(alignedAddress) - sizeof(AllocationHeader));

    header->m_adjustment      = adjustment;
    header->m_previousAddress = m_previousPosition;
    
    m_previousPosition = alignedAddress;
    m_currentPosition  = static_cast<uint8_t *>(alignedAddress) + size;

    m_usedMemory += size + adjustment;
    ++m_numAllocations;

    return alignedAddress;
}

//------------------------------------------------------------------------------
void StackMemoryManager::free(void * p)
{
    if (p != m_previousPosition)
    {
        // Error - Can only free in LIFO order
        const std::string msg(" Can only free in LIFO order");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the header in the bytes preceding the pointer
    AllocationHeader * header = reinterpret_cast<AllocationHeader *>(static_cast<uint8_t *>(p) - sizeof(AllocationHeader));

    m_usedMemory -= static_cast<uint8_t *>(m_currentPosition) - static_cast<uint8_t *>(p) + header->m_adjustment;
    m_currentPosition  = static_cast<uint8_t *>(p) - header->m_adjustment;
    m_previousPosition = header->m_previousAddress;

    --m_numAllocations;
}
