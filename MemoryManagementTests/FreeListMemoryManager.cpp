
// Project Includes
#include "FreeListMemoryManager.h"

// Common Libary
#include "Exception.h"

// Standard Includes
#include <string>

//------------------------------------------------------------------------------
FreeListMemoryManager::FreeListMemoryManager(size_t size)
    :
    m_size(size)
  , m_start(nullptr)
  , m_usedMemory(0)
  , m_numAllocations(0)
  , m_freeBlocks(nullptr)
{
    if (size <= 0)
    {
        // Error - Invalid size
        const std::string msg("Invalid size requested. Size must be greater than zero.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_start = ::malloc(size);

    if( !m_start )
    {
        // Error - System failed to allocate requested size
        const std::string msg("System failed to allocate requested size");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // To start with, all the memory is in one free block
    m_freeBlocks = static_cast<FreeBlock *>(m_start);
    m_freeBlocks->m_size = size;
    m_freeBlocks->m_next = nullptr;
}

//------------------------------------------------------------------------------
FreeListMemoryManager::~FreeListMemoryManager()
{
    ::free(m_start);
}

//------------------------------------------------------------------------------
void * FreeListMemoryManager::allocate(size_t size, uint8_t alignment)
{
    if( size <= 0 )
    {
        // Error - Invalid size
        const std::string msg("Invalid size requested. Size must be greater than zero.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( !alignment )
    {
        // Error - Invalid alignment
        const std::string msg("Invalid alignment. Alignment must be greater than zero.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Iterate over the free blocks and look for the first that has enough space to fit the request
    FreeBlock * previousFreeBlock = nullptr;
    FreeBlock * freeBlock         = m_freeBlocks;

    while( freeBlock != nullptr )
    {
        // Calculate adjustment needed to keep object correctly aligned
        size_t availableSpace = freeBlock->m_size;
        void * address = reinterpret_cast<void *>(freeBlock);

        void * alignedAddress = std::align(alignment, size, address, availableSpace);

        if( !alignedAddress )
        {
            // Does not fit
            freeBlock = freeBlock->m_next;
            continue;
        }

        // Check if the space given by the alignment was enough to contain the header
        uint8_t adjustment = static_cast<int8_t>(static_cast<uint8_t *>(alignedAddress) - static_cast<uint8_t *>(address));
        uint8_t neededSpace = sizeof(AllocationHeader);

        if( adjustment < neededSpace )
        {
            neededSpace -= adjustment;

            // Increase adjustment to fit header
            adjustment += alignment * (neededSpace / alignment);

            if (neededSpace % alignment > 0)
            {
                adjustment += alignment;
            }
        }

        // Check that the requested size and the space we created for alignment and header will fit
        size_t totalSize = adjustment + size;

        if( totalSize > freeBlock->m_size )
        {
            // Does not fit
            freeBlock = freeBlock->m_next;
            continue;
        }

        alignedAddress = static_cast<uint8_t *>(address) + adjustment;

        // Check If allocations in the remaining memory will be impossible
        if( freeBlock->m_size - totalSize <= sizeof(AllocationHeader) )
        {
            // Cannot fit any additional allocations in this block
            // Take the rest of the available space in this block, so that it doesn't sit unused.
            // When this block is freed then it might become useful again.
            totalSize = freeBlock->m_size;

            if( previousFreeBlock != nullptr )
            {
                previousFreeBlock->m_next = freeBlock->m_next;
            }
            else
            {
                m_freeBlocks = freeBlock->m_next;
            }
        }
        else
        {
            // We could possibly fit additional allocation into the remaining space
            // Create a new FreeBlock containing remaining memory and insert it into the list
            FreeBlock * nextBlock = reinterpret_cast<FreeBlock *>(reinterpret_cast<uint8_t *>(freeBlock) + totalSize);
            
            nextBlock->m_size = freeBlock->m_size - totalSize;
            nextBlock->m_next = freeBlock->m_next;

            if( previousFreeBlock != nullptr )
            {
                previousFreeBlock->m_next = nextBlock;
            }
            else
            {
                m_freeBlocks = nextBlock;
            }
        }

        AllocationHeader * header = reinterpret_cast<AllocationHeader *>(static_cast<uint8_t *>(alignedAddress) - sizeof(AllocationHeader));
        header->m_size       = totalSize;
        header->m_adjustment = adjustment;

        m_usedMemory += totalSize;
        ++m_numAllocations;

        return alignedAddress;
    }

    const std::string msg("No free space large enough to accomodate requested size was found.");
    throw Common::Exception(__FILE__, __LINE__, msg);
}

//------------------------------------------------------------------------------
void FreeListMemoryManager::free(void * p)
{
    if( !p )
    {
        // Error - p is nullptr
        const std::string msg("p is nullptr");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    AllocationHeader * header = reinterpret_cast<AllocationHeader *>(static_cast<uint8_t *>(p) - sizeof(AllocationHeader));

    uint8_t * blockStart = static_cast<uint8_t *>(p) - header->m_adjustment;
    size_t    blockSize  =  header->m_size;
    uint8_t * blockEnd   =  blockStart + blockSize;

    FreeBlock * previousFreeBlock = nullptr;
    FreeBlock * freeBlock = m_freeBlocks;

    while( freeBlock != nullptr )
    {
        if( reinterpret_cast<uint8_t *>(freeBlock) >= blockEnd )
        {
            break;
        }

        previousFreeBlock = freeBlock;
        freeBlock = freeBlock->m_next;
    }

    if( !previousFreeBlock )
    {
        previousFreeBlock = reinterpret_cast<FreeBlock *>(blockStart);
        previousFreeBlock->m_size = blockSize;
        previousFreeBlock->m_next = m_freeBlocks;

        m_freeBlocks = previousFreeBlock;
    }
    else if( reinterpret_cast<uint8_t *>(previousFreeBlock) + previousFreeBlock->m_size == blockStart )
    {
        previousFreeBlock->m_size += blockSize;
    }
    else
    {
        FreeBlock * temp = reinterpret_cast<FreeBlock *>(blockStart);
        temp->m_size = blockSize;
        temp->m_next = previousFreeBlock->m_next;
        previousFreeBlock->m_next = temp;
        previousFreeBlock = temp;
    }

    if( freeBlock != nullptr && reinterpret_cast<uint8_t *>(freeBlock) == blockEnd )
    {
        previousFreeBlock->m_size += freeBlock->m_size;
        previousFreeBlock->m_next = freeBlock->m_next;
    }

    --m_numAllocations;
    m_usedMemory -= blockSize;
}

//------------------------------------------------------------------------------
