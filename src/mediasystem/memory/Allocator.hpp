//
//  AllocatorHost.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "AllocatorTraits.hpp"
#include "AllocationManager.hpp"

namespace mediasystem {
    
    template<typename T>
    class std_Allocator : public std::allocator<T> {
    public:
        ALLOCATOR_TRAITS(T);
        explicit std_Allocator(AllocationManager* manager = nullptr, const AllocationPolicyFormat& fmt = AllocationPolicyFormat()){}
        
        template<typename U>
        struct rebind
        {
            typedef std_Allocator<U> other;
        };
        
        template<typename U>
        std_Allocator(std_Allocator<U> const& other): std::allocator<T>(other){}
    };
    
    template<typename T>
    class ms_Allocator {
    public:
        ALLOCATOR_TRAITS(T);
        
        explicit ms_Allocator(AllocationManager* manager = nullptr, const AllocationPolicyFormat& fmt = AllocationPolicyFormat()):mManager(manager){
            if(mManager){
                auto policy = mManager->getPolicy<T>();
                if(!policy){
                    mManager->setPolicy<T>(fmt);
                }
            }
        }
        
        template<typename U>
        struct rebind
        {
            typedef ms_Allocator<U> other;
        };
        
        template<typename U>
        ms_Allocator(ms_Allocator<U> const& other) :
            mManager(other.mManager)
        {
            if(!mManager)
                throw std::bad_alloc();
            
            if(auto mypolicy = mManager->getPolicy<T>()){
                return;
            }else{
                if(auto policy = mManager->getPolicy<U>()){
                    std::stringstream fmtStream;
                    fmtStream << policy->getFormat();
                    ofLogVerbose("Memory") << "rebinding alloctor for type T [size: " << sizeof(T) <<" id: " << typeid(T).name() << "]\n"
                    << "from type U [size: " << sizeof(U) <<" id: " << typeid(U).name() << "]\n"
                    << "with"
                    << fmtStream.str();
                    mManager->trySetPolicy<T>(policy->getFormat());
                }else{
                    ofLogVerbose("Memory") << "don't have policies for U [id: " << typeid(U).name() << "]\n"
                    << "or T [id: " << typeid(T).name() << "]\n"
                    << "defaulting both to heap";
                    mManager->setPolicy<T>(); //default
                    mManager->setPolicy<U>(); //default
                }
            }
            
        }
        
        pointer allocate(size_type count = 1, const_pointer hint = 0)
        {
            if(!mManager)
                throw std::bad_alloc();
            auto policy = mManager->getPolicy<T>();
            return static_cast<pointer>(policy->allocate(count));
        }
        
        void deallocate(pointer ptr, size_type count = 1)
        {
            if(!mManager)
                throw std::bad_alloc();
            auto policy = mManager->getPolicy<T>();
            policy->deallocate(ptr, count);
        }
        
        //These don't work on all platforms
        type*       address(type&       obj) const {return &obj;}
        type const* address(type const& obj) const {return &obj;}
        
        template<typename...Args>
        void construct(type* ptr, Args&&...args)
        {
            new(ptr) type(args...);
        }
        
        void destroy(type* ptr)
        {
            ptr->~type();
        }
        
        AllocationManager* mManager;
    };
    
}//end namespace mediasystem

// Two allocators are not equal unless a specialization says so
template<typename T>
bool operator==(mediasystem::ms_Allocator<T> const& left, mediasystem::ms_Allocator<T> const& right)
{
    return true;
}

// Two allocators are not equal unless a specialization says so
template<typename T>
bool operator!=(mediasystem::ms_Allocator<T> const& left, mediasystem::ms_Allocator<T> const& right)
{
    return !(left == right);
}

// Two allocators are not equal unless a specialization says so
template<typename T,typename U>
bool operator==(mediasystem::ms_Allocator<T> const& left, mediasystem::ms_Allocator<U> const& right)
{
	return false;
}

// Also implement inequality
template<typename T, typename U>
bool operator!=(mediasystem::ms_Allocator<T> const& left, mediasystem::ms_Allocator<U> const& right)
{
	return !(left == right);
}

// Comparing an allocator to anything else should not show equality
template<typename T, typename OtherAllocator>
bool operator==(mediasystem::ms_Allocator<T> const& left, OtherAllocator const& right)
{
	return false;
}

// Also implement inequality
template<typename T, typename OtherAllocator>
bool operator!=(mediasystem::ms_Allocator<T> const& left, OtherAllocator const& right)
{
	return !(left == right);
}


// Two allocators are not equal unless a specialization says so
template<typename T>
bool operator==(mediasystem::std_Allocator<T> const& left, mediasystem::std_Allocator<T> const& right)
{
    return &left == &right;
}

// Two allocators are not equal unless a specialization says so
template<typename T>
bool operator!=(mediasystem::std_Allocator<T> const& left, mediasystem::std_Allocator<T> const& right)
{
    return !(left == right);
}

// Two allocators are not equal unless a specialization says so
template<typename T,typename U>
bool operator==(mediasystem::std_Allocator<T> const& left, mediasystem::std_Allocator<U> const& right)
{
    return false;
}

// Also implement inequality
template<typename T, typename U>
bool operator!=(mediasystem::std_Allocator<T> const& left, mediasystem::std_Allocator<U> const& right)
{
    return !(left == right);
}

// Comparing an allocator to anything else should not show equality
template<typename T, typename OtherAllocator>
bool operator==(mediasystem::std_Allocator<T> const& left, OtherAllocator const& right)
{
    return false;
}

// Also implement inequality
template<typename T, typename OtherAllocator>
bool operator!=(mediasystem::std_Allocator<T> const& left, OtherAllocator const& right)
{
    return !(left == right);
}

