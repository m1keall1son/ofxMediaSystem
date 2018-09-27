//
//  IAllocator.h
//  MemoryManagement
//
//  Created by Michael Allison on 4/6/18.
//

#pragma once

#include <stdint.h>
#include <cstddef>

namespace mediasystem {

template<typename T>
struct max_allocations
{
    enum{value = static_cast<size_t>(-1) / sizeof(T)};
};

}//end namspace mediasystem
