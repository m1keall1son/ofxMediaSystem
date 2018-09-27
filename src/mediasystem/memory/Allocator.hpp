//
//  AllocatorHost.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once

#include "DefaultInitializer.hpp"
#include "AllocatorTraits.hpp"
#include "HeapAllocator.hpp"

#define FORWARD_ALLOCATOR_TRAITS(C)                  \
typedef typename C::value_type      value_type;      \
typedef typename C::pointer         pointer;         \
typedef typename C::const_pointer   const_pointer;   \
typedef typename C::reference       reference;       \
typedef typename C::const_reference const_reference; \
typedef typename C::size_type       size_type;       \
typedef typename C::difference_type difference_type; \

namespace mediasystem {

template<typename T,
	typename AllocationPolicy = HeapAllocator<T>,
	typename InitializationPolicy = DefaultInitializer<T> >
class Allocator : public AllocationPolicy, public InitializationPolicy
{
public:
	
	// Template parameters
	typedef AllocationPolicy Policy;
    typedef InitializationPolicy Initailization;
	
	FORWARD_ALLOCATOR_TRAITS(Policy)
	
	template<typename U>
	struct rebind
	{
		typedef Allocator<U,
		typename Policy::template rebind<U>::other,
		typename Initailization::template rebind<U>::other
		> other;
	};
	
	template<typename...Args>
	Allocator(Args&&...args) : Policy(std::forward<Args>(args)...), Initailization(std::forward<Args>(args)...) {}
	
	// Copy Constructor
	template<typename U,
	typename AllocationPolicyU,
	typename InitializationAllocationPolicyU>
	Allocator(Allocator<U,
			  AllocationPolicyU,
			  InitializationAllocationPolicyU> const& other) :
	Policy(other),
	Initailization(other)
	{}
};
    
}//end namespace mediasystem

// Two allocators are not equal unless a specialization says so
template<typename T, typename AllocationPolicy, typename InitializationPolicy,
typename U, typename AllocationPolicyU, typename InitializationAllocationPolicyU>
bool operator==(mediasystem::Allocator<T, AllocationPolicy, InitializationPolicy> const& left,
                mediasystem::Allocator<U, AllocationPolicyU, InitializationAllocationPolicyU> const& right)
{
	return false;
}

// Also implement inequality
template<typename T, typename AllocationPolicy, typename InitializationPolicy,
typename U, typename AllocationPolicyU, typename InitializationAllocationPolicyU>
bool operator!=(mediasystem::Allocator<T, AllocationPolicy, InitializationPolicy> const& left,
				mediasystem::Allocator<U, AllocationPolicyU, InitializationAllocationPolicyU> const& right)
{
	return !(left == right);
}

// Comparing an allocator to anything else should not show equality
template<typename T, typename AllocationPolicy, typename InitializationPolicy,
typename OtherAllocator>
bool operator==(mediasystem::Allocator<T, AllocationPolicy, InitializationPolicy> const& left,
				OtherAllocator const& right)
{
	return false;
}

// Also implement inequality
template<typename T, typename AllocationPolicy, typename InitializationPolicy,
typename OtherAllocator>
bool operator!=(mediasystem::Allocator<T, AllocationPolicy, InitializationPolicy> const& left,
				OtherAllocator const& right)
{
	return !(left == right);
}
    

