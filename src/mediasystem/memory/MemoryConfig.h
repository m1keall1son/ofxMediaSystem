//
//  MemoryConfig.h
//  Belron
//
//  Created by Michael Allison on 9/25/18.
//

#pragma once

#include "PoolAllocator.hpp"
#include "HeapAllocator.hpp"
#include "Allocator.hpp"

namespace mediasystem {
    
    constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE = 1024; //1kb
    
    template<typename T, size_t BlockSize = DEFAULT_MEMORY_BLOCK_SIZE>
    using DynamicAllocator = Allocator<T,PoolAllocator<T,BlockListStorage,BlockSize>>;
    
}//end namespace mediasystem