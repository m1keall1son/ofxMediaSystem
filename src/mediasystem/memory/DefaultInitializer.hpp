//
//  ObjectTraits.hpp
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once

namespace mediasystem {

template<typename T>
class DefaultInitializer
{
public:
	
	typedef T type;
	
	template<typename U>
	struct rebind
	{
		typedef DefaultInitializer<U> other;
	};
	
	// Constructor
	DefaultInitializer(void){}
	
	// Copy Constructor
	template<typename U>
	DefaultInitializer(DefaultInitializer<U> const& other){}
	
	// Address of object
	type*       address(type&       obj) const {return &obj;}
	type const* address(type const& obj) const {return &obj;}
		
	// Construct object
	template<typename...Args>
	void construct(type* ptr, Args&&...args)
	{
		new(ptr) type(args...);
	}

	// Destroy object
	void destroy(type* ptr)
	{
		// Call destructor
		ptr->~type();
	}
};
    
}//end namespace mediasystem
