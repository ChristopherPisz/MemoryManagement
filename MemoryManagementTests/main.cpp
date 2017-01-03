
// Project Includes
#include "ComplexNumber.h"
#include "CustomAllocator.hxx"
#include "FreeListMemoryManager.h"
#include "LinearMemoryManager.h"
#include "StackMemoryManager.h"

// Common Library
#include "PerformanceTimer.h"

// Standard Includes
#include <iostream>
#include <vector>

//------------------------------------------------------------------------------
const size_t g_numElements(1000);
const size_t g_iterations(5000);

//------------------------------------------------------------------------------
void RunNoMemoryManagement()
{
    // Start timer
    Common::PerformanceTimer timer;
    timer.Start();

    // Do the work
    for (int i = 0; i < g_iterations; ++i)
    {
        ComplexNumber * array[g_numElements];

        for (int j = 0; j < g_numElements; ++j)
        {
            array[j] = new ComplexNumber(i, j);
        }

        for (int j = 0; j < g_numElements; ++j)
        {
            delete array[j];
        }
    }

    // Stop the timer
    double secondsElapsed = timer.Stop();
    std::cout << "Test with no memory management took " << secondsElapsed << " seconds.\n";
}

//------------------------------------------------------------------------------
void RunLinearMemoryManagement()
{
    // Start timer
    Common::PerformanceTimer timer;
    timer.Start();

    // Do the work
    {
        for (int i = 0; i < g_iterations; ++i)
        {
            LinearMemoryManager pool(sizeof(ComplexNumber) * g_numElements);
            ComplexNumber * array[g_numElements];

            // Allocate
            for (int j = 0; j < g_numElements; ++j)
            {
                void * address = pool.allocate(sizeof(ComplexNumber), alignof(ComplexNumber));
                array[j] = new(address) ComplexNumber(i, j);
            }

            // Must release all at once
            pool.clear();
        }
    }

    // Stop the timer
    double secondsElapsed = timer.Stop();
    std::cout << "Test with linear memory management took " << secondsElapsed << " seconds.\n";
}

//------------------------------------------------------------------------------
void RunStackMemoryManagement()
{
    // Start timer
    Common::PerformanceTimer timer;
    timer.Start();

    // Do the work
    {
        for (int i = 0; i < g_iterations; ++i)
        {
            // We need to add, in the worst case, 2x the size of the header for each element
            // (because we need to still allow for the alignment of our type)
            StackMemoryManager pool((sizeof(ComplexNumber) + 32) * g_numElements);
            ComplexNumber * array[g_numElements];

            // Allocate
            for (int j = 0; j < g_numElements; ++j)
            {
                void * address = pool.allocate(sizeof(ComplexNumber), alignof(ComplexNumber));
                array[j] = new(address) ComplexNumber(i, j);
            }

            // Must release in LIFO order
            for (int j = g_numElements - 1; j >= 0; --j)
            {
                pool.free(array[j]);
            }
        }
    }

    // Stop the timer
    double secondsElapsed = timer.Stop();
    std::cout << "Test with linear memory management took " << secondsElapsed << " seconds.\n";
}

//------------------------------------------------------------------------------
void RunFreeListMemoryManagement()
{
    // Start timer
    Common::PerformanceTimer timer;
    timer.Start();

    // Do the work
    {
        for( int i = 0; i < g_iterations; ++i )
        {
            // We need to add, in the worst case, 2x the size of the header for each element
            // (because we need to still allow for the alignment of our type)
            FreeListMemoryManager pool((sizeof(ComplexNumber) + 32) * g_numElements);
            ComplexNumber * array[g_numElements];

            // Allocate
            for( int j = 0; j < g_numElements; ++j )
            {
                void * address = pool.allocate(sizeof(ComplexNumber), alignof(ComplexNumber));
                array[j] = new(address) ComplexNumber(i, j);
            }

            // Free
            for( int j = 0; j < g_numElements; ++j )
            {
                pool.free(array[j]);
            }
        }
    }

    // Stop the timer
    double secondsElapsed = timer.Stop();
    std::cout << "Test with linear memory management took " << secondsElapsed << " seconds.\n";
}

//------------------------------------------------------------------------------
void RunDefaultAllocator()
{
    // Start timer
    Common::PerformanceTimer timer;
    timer.Start();

    // Do the Work
    {
        std::vector<ComplexNumber> myVector;

        for (int i = 0; i < g_iterations; i++)
        {
            for (int j = 0; j < g_numElements; j++)
            {
                myVector.push_back(ComplexNumber(i, j));
            }
        }
    }

    // Stop the timer
    double secondsElapsed = timer.Stop();
    std::cout << "Test with default allocator took " << secondsElapsed << " seconds.\n";
}

//------------------------------------------------------------------------------
/*
void RunCustomAllocator()
{
    // Start timer
    Common::PerformanceTimer timer;
    timer.Start();

    // Do the Work
    IMemoryManager * memoryManager = new SimpleMemoryManager();

    // We have to control the vector going out of scope before the memory manager goes out of scope
    {
        CustomAllocator<ComplexNumber> allocator(memoryManager);
        std::vector<ComplexNumber, CustomAllocator<ComplexNumber> > myVector(allocator);

        for (int i = 0; i < g_iterations; i++)
        {
            for (int j = 0; j < g_numElements; j++)
            {
                myVector.push_back(ComplexNumber(i, j));
            }
        }
    }

    delete memoryManager;

    // Stop the timer
    double secondsElapsed = timer.Stop();
    std::cout << "Test with custom allocator using simple memory manager took " << secondsElapsed << " seconds.\n";
}
*/

//------------------------------------------------------------------------------
int main(int argc, char * argv[])
{
    RunNoMemoryManagement();
//  RunLinearMemoryManagement();
//  RunStackMemoryManagement();
    RunFreeListMemoryManagement();

//  RunCustomAllocator();

    return 0;
}