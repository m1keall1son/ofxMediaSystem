//
//  AllocatorTraits.hpp
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once

#define ALLOCATOR_TRAITS(T)                \
typedef T                 type;            \
typedef type              value_type;      \
typedef value_type*       pointer;         \
typedef value_type const* const_pointer;   \
typedef value_type&       reference;       \
typedef value_type const& const_reference; \
typedef std::size_t       size_type;       \
typedef std::ptrdiff_t    difference_type; \

