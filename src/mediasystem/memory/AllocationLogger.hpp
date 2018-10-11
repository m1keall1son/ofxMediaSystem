//
//  AllocationLogger.hpp
//  Belron
//
//  Created by Michael Allison on 10/9/18.
//

#pragma once

#include "AllocatorTraits.hpp"
#include "IAllocator.h"
#include "ofMain.h"
#include <chrono>

template<typename T, template <typename> class Alloc>
class AllocationLogger  {
public:
    
    ALLOCATOR_TRAITS(T)
    
    template<typename U>
    struct rebind
    {
        typedef AllocationLogger<U,Alloc> other;
    };
    
    AllocationLogger() = default;
    
    template<typename U>
    AllocationLogger(AllocationLogger<U,Alloc> const& other){}
    
    pointer allocate(size_type count = 1, const_pointer hint = 0)
    {
        auto ret = mAllocator.allocate(count,hint);
        ofLogVerbose("Memory") << "Allocation: " << typeid(T).name() << "\n"
        << "\tbytes: " << count * sizeof(T) << "\n"
        << "\tlocation: " << ret << "\n"
        << "\tT size: " << sizeof(T) << "\n"
        << "\tT count: " << count
        << "\ttotal allocations of T: " << ++sAllocationsCount
        << "\tcurrent allocations of T: " << ++sCurrentAllocationsCount;
        return ret;
    }
    
    void deallocate(pointer ptr, size_type count = 1)
    {
        ofLogVerbose("Memory")  << "Deallocation: " << typeid(T).name() << "\n"
        << "\tbytes: " << count * sizeof(T) << "\n"
        << "\tlocation: " << ptr << "\n"
        << "\tT size: " << sizeof(T) << "\n"
        << "\tT count: " << count
        << "\ttotal deallocations of T: " << --sDeallocationsCount
        << "\tcurrent allocations of T: " << --sCurrentAllocationsCount;
        mAllocator.deallocate(ptr,count);
    }
    
    // Max number of objects that can be allocated in one call
    size_type max_size(void) const {return mAllocator.max_size();}
    size_t capacity(){ return mAllocator.capacity(); }
    
private:
    
    static size_t sAllocationsCount{0};
    static size_t sDeallocationsCount{0};
    static size_t sCurrentAllocationsCount{0};
    Alloc<T> mAllocator;
    
};
