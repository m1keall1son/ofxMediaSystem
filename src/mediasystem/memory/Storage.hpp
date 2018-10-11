//
//  FreeStore.hpp
//  MemoryManagement
//
//  Created by Michael Allison on 4/6/18.
//

#pragma once

#include <cstring>
#include <stdint.h>
#include <memory>
#include "IAllocator.h"

namespace mediasystem {

struct InBytes {
    static constexpr size_t objectCount(size_t size, size_t objectSize){ return size / objectSize; }
};

struct InNumObjects {
    static constexpr size_t objectCount(size_t size, size_t objectSize){ return size; }
};
    
class IMemoryStorage {
public:
    virtual void* operator[](size_t index) = 0;
    virtual bool canGrow() const = 0;
    virtual size_t capacity() const = 0;
    virtual size_t maxSize() const = 0;
};

template<typename T, size_t Size, typename SizeKind = InBytes>
    class FixedSizeStorage : public IMemoryStorage {
public:
    
    static_assert( std::is_same<SizeKind,InBytes>::value || std::is_same<SizeKind,InNumObjects>::value, "SizeKind must be either InBytes or InNumObjects");
    
    constexpr static const size_t OBJECT_SIZE = ((sizeof(T) + sizeof(void *)-1) / sizeof(void *)) * sizeof(void *);
    constexpr static const size_t OBJECTS_PER_BLOCK = SizeKind::objectCount(Size, OBJECT_SIZE);
    constexpr static const size_t BLOCK_SIZE = OBJECTS_PER_BLOCK * OBJECT_SIZE;
    
    FixedSizeStorage() :
    mObjects(::operator new(BLOCK_SIZE))
    {
        std::cout << "new block of size: " << BLOCK_SIZE << std::endl;
        std::cout << "object size: " << OBJECT_SIZE << std::endl;
        std::cout << "allowed num objects per block: " << OBJECTS_PER_BLOCK << std::endl;
        std::memset(mObjects, 0, BLOCK_SIZE);
    }
    
    ~FixedSizeStorage() {
        ::operator delete( mObjects );
    }
    
    void* operator[](size_t index) override {
        if(index >= OBJECTS_PER_BLOCK) throw std::bad_alloc();
        char * head = reinterpret_cast<char*>(mObjects);
        return reinterpret_cast<void*>(head + (index*OBJECT_SIZE));
    }
    
    bool canGrow() const override { return false; }
    size_t capacity() const override { return OBJECTS_PER_BLOCK; }
    size_t maxSize() const override { return BLOCK_SIZE; }

private:
   void* mObjects;
};

template<typename T, size_t Size, typename SizeKind = InBytes>
class BlockListStorage : public IMemoryStorage {
public:
    
    static_assert( std::is_same<SizeKind,InBytes>::value || std::is_same<SizeKind,InNumObjects>::value, "SizeKind must be either InBytes or InNumObjects");
    
    constexpr static const size_t OBJECT_SIZE = ((sizeof(T) + sizeof(void *)-1) / sizeof(void *)) * sizeof(void *);
    constexpr static const size_t OBJECTS_PER_BLOCK = SizeKind::objectCount(Size,OBJECT_SIZE);
    constexpr static const size_t BLOCK_SIZE = OBJECTS_PER_BLOCK * OBJECT_SIZE;
    
    BlockListStorage() : mBlocks(1) {}
    ~BlockListStorage(){ mBlocks.clear(); }
    
    void* operator[](size_t index) override {
        size_t block = floor(index / OBJECTS_PER_BLOCK);
        if(block == mBlocks.size()){
            std::cout << "exceeded block count, create a new one" << std::endl;
            mBlocks.emplace_back();
        }else if(block > mBlocks.size()){
            for(size_t i = 0; i < (block - mBlocks.size()); i++ ){
                std::cout << "catching up..." << std::endl;
                mBlocks.emplace_back();
            }
        }
        auto it = mBlocks.begin();
        std::advance(it, block);
        return (*it)[index % OBJECTS_PER_BLOCK];
    }
    
    bool canGrow() const override { return true; }
    size_t capacity() const override { return mBlocks.size() * OBJECTS_PER_BLOCK; }
    size_t maxSize() const override { return mBlocks.size() * BLOCK_SIZE; }
    
private:
    std::list<FixedSizeStorage<T,Size,SizeKind>> mBlocks;
};
    
}//end namespace mediasystem
