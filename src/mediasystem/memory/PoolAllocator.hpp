
#pragma once

//
//  PoolAllocator.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#include <exception>
#include <list>
#include <iostream>
#include "Storage.hpp"

namespace mediasystem {
    
class cant_allocate_array : public std::bad_alloc {};

template<typename T, template<typename,size_t,typename> class StorageType, size_t StorageSize, typename StorageSizeKind = InBytes>
class PoolAllocator
{
    
    static_assert(std::is_base_of<IMemoryStorage,StorageType<T,StorageSize,StorageSizeKind>>::value, "StorageType must derive from IMemoryStorage");
    
    class MemoryPool {
    public:
        void* allocate(){
            void* ret;
            if(mFreeStore){
                ret = mFreeStore;
                auto next = *reinterpret_cast<void**>(mFreeStore);
                mFreeStore = next;
            }else{
                ret = mStorage[mLast++];
            }
            return ret;
        }
        
        void deallocate(void* ptr){
            *reinterpret_cast<void**>(ptr) = mFreeStore;
            mFreeStore = ptr;
        }
        
        bool canGrow() const { return mStorage.canGrow(); }
        size_t capacity() const { return mStorage.capacity(); }

    private:
        void* mFreeStore{nullptr};
        size_t mLast{0};
        StorageType<T,StorageSize,StorageSizeKind> mStorage;
    };
    
public:
    
    ALLOCATOR_TRAITS(T)
    
    template<typename U>
    struct rebind
    {
        typedef PoolAllocator<U,StorageType,StorageSize,StorageSizeKind> other;
    };
    
    PoolAllocator() = default;
    
    // Copy Constructor for rebinds
    template<typename U>
    PoolAllocator(PoolAllocator<U,StorageType,StorageSize,StorageSizeKind> const& other){}
    
    // Allocate memory from pool
    pointer allocate(size_type count = 1, const_pointer hint = 0)
    {
        if(count == 1){
            return static_cast<pointer>(sPool.allocate());
        }else{
           return static_cast<pointer>(::operator new(count * sizeof(type), ::std::nothrow));
        }
    }
    
    // place freed memory back into the pool
    void deallocate(pointer ptr, size_type count = 1)
    {
        if(count == 1){
            sPool.deallocate(static_cast<void*>(ptr));
        }else{
            ::operator delete(ptr);
        }
    }
    
    bool canGrow() const { return sPool.canGrow(); }
    size_t maxAllocationCount() const { return 1; }
    size_t capacity()const { return sPool.capacity(); }
    
private:
    
    static MemoryPool sPool;
};
    
}//end namespace mediasystem

template<typename T, template<typename,size_t,typename> class StorageType, size_t StorageSize, typename StorageSizeKind>
typename mediasystem::PoolAllocator<T,StorageType,StorageSize,StorageSizeKind>::MemoryPool mediasystem::PoolAllocator<T,StorageType,StorageSize,StorageSizeKind>::sPool;

