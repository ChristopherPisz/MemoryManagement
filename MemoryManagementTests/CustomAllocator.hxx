#pragma once

// Project Includes
#include "IMemoryManager.h"

// Standard Includes
#include <iostream>
#include <type_traits>
#include <memory>

//------------------------------------------------------------------------------
/// <summary>
/// A STL compliant custom allocator
///
/// Based on Dr Dobbs article
/// http://www.drdobbs.com/the-standard-librarian-what-are-allocato/184403759
/// I attempted to make it C++11 compliant.
/// </summary>
template <class T>
class CustomAllocator
{
protected:

    IMemoryManager * m_memoryManager;

public:

    template <typename U> friend class CustomAllocator;
    typedef T value_type;

    /*
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap            = std::false_type;
    */

    /// Allocator takes ownership of memory manager
    explicit CustomAllocator(IMemoryManager * memoryManager)
        :
        m_memoryManager(memoryManager)
    {
    }

    template <class U>
    CustomAllocator(const CustomAllocator<U> & rhs)
        :
        m_memoryManager(rhs.m_memoryManager)
    {
    }

    ~CustomAllocator()
    {
    }

    bool operator == (const CustomAllocator<T> & rhs)
    {
        return m_memoryManager == rhs.m_memoryManager;
    }

    bool operator != (const CustomAllocator<T> & rhs)
    {
        return m_memoryManager != rhs.m_memoryManager;
    }

    T * allocate(size_t n)
    {
        const size_t numBytes = n * sizeof(T);
        void * p = m_memoryManager->allocate(numBytes);

        if (!p)
        {
            throw std::bad_alloc();
        }

        // std::cout << "Allocated " << numBytes << " bytes for a " << typeid(T).name() << std::endl;
        return static_cast<T *>(p);
    }

    void deallocate(T * p, size_t numObjects)
    {
        const size_t numBytes = sizeof(*p);
        m_memoryManager->free(p);

        // std::cout << "Deallocated " << numBytes << " bytes for a " << typeid(T).name() << std::endl;
    }
};
