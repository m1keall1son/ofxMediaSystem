//
//  AllocatorHost.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once

#include "AllocatorTraits.hpp"
#include "AllocationManager.hpp"

namespace mediasystem {
    
    template<typename T>
    class _Allocator {
    public:
        ALLOCATOR_TRAITS(T);
        
        explicit _Allocator(AllocationManager* manager = nullptr):mManager(manager){
            if(mManager){
                auto policy = mManager->getPolicy<T>();
                if(!policy){
                    mManager->addPolicy<T>(AllocationPolicyFormat(), false);
                }
            }
        }
        
        template<typename U>
        struct rebind
        {
            typedef _Allocator<U> other;
        };
        
        template<typename U>
        _Allocator(_Allocator<U> const& other) :
            mManager(other.mManager)
        {
            if(!mManager)
                throw std::bad_alloc();
            
            if(auto mypolicy = mManager->getPolicy<T>()){
                return;
            }else{
                if(auto policy = mManager->getPolicy<U>()){
                    std::cout << "constructing T - size: " << sizeof(T) <<" type: " << typeid(T).name() << "\n";
                    std::cout << "from U - size: " << sizeof(U) <<" type: " << typeid(U).name() << std::endl;
                    std::cout << "with \n";
                    std::cout << policy->getFormat();
                    mManager->addPolicy<T>(policy->getFormat(), false);
                }else{
                    std::cout << "don't have policies for U type: " << typeid(U).name() << "\n";
                    std::cout << "or T type: " << typeid(T).name() << std::endl;
                    std::cout << "defaulting both to heap" << std::endl;
                    mManager->addPolicy<T>( AllocationPolicyFormat(), false ); //default
                    mManager->addPolicy<U>( AllocationPolicyFormat(), false ); //default
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
bool operator==(mediasystem::_Allocator<T> const& left, mediasystem::_Allocator<T> const& right)
{
    return left.mManager == right.mManager;
}

// Two allocators are not equal unless a specialization says so
template<typename T>
bool operator!=(mediasystem::_Allocator<T> const& left, mediasystem::_Allocator<T> const& right)
{
    return !(left == right);
}

// Two allocators are not equal unless a specialization says so
template<typename T,typename U>
bool operator==(mediasystem::_Allocator<T> const& left, mediasystem::_Allocator<U> const& right)
{
	return false;
}

// Also implement inequality
template<typename T, typename U>
bool operator!=(mediasystem::_Allocator<T> const& left, mediasystem::_Allocator<U> const& right)
{
	return !(left == right);
}

// Comparing an allocator to anything else should not show equality
template<typename T, typename OtherAllocator>
bool operator==(mediasystem::_Allocator<T> const& left, OtherAllocator const& right)
{
	return false;
}

// Also implement inequality
template<typename T, typename OtherAllocator>
bool operator!=(mediasystem::_Allocator<T> const& left, OtherAllocator const& right)
{
	return !(left == right);
}
    

