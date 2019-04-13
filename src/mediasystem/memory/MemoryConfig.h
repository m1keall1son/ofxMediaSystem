//
//  MemoryConfig.h
//  Belron
//
//  Created by Michael Allison on 9/25/18.
//

#pragma once

#include "Allocator.hpp"

#if !defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
#define MS_ALLOW_ALLOCATION_MIDDLEWARE
#endif

namespace mediasystem {
    
    template<typename T>
    using Allocator = ms_Allocator<T>;
    
    //To configure the memory subsystem to use the std library default allocator
    //comment out the allocator alias above and uncomment the alias below:
    
    //template<typename T>
    //using Allocator = std_Allocator<T>;
    
}//end namespace mediasystem
