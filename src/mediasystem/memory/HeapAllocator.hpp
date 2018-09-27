//
//  HeapPolicy.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once
#include "AllocatorTraits.hpp"
#include "IAllocator.h"
#include <new>

namespace mediasystem {

template<typename T>
class HeapAllocator
{
public:
	
	ALLOCATOR_TRAITS(T)
	
	template<typename U>
	struct rebind
	{
		typedef HeapAllocator<U> other;
	};
	
	// Default Constructor
	HeapAllocator() = default;
	
	// Copy Constructor
	template<typename U>
	HeapAllocator(HeapAllocator<U> const& other){}
	
	// Allocate memory
	pointer allocate(size_type count, const_pointer hint = 0)
	{
		if(count > maxAllocationCount()){throw std::bad_alloc();}
		return static_cast<pointer>(::operator new(count * sizeof(type), ::std::nothrow));
	}
	
	// Delete memory
	void deallocate(pointer ptr, size_type count)
	{
		::operator delete(ptr);
	}
	
	size_t maxAllocationCount() const {return max_allocations<T>::value;}
    size_t capacity() const { return maxAllocationCount(); }
};

}//end namespace mediasystem
