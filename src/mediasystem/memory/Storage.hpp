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

namespace mediasystem {
    
    enum AllocationStorageType { FIXED_SIZE_STORAGE, BLOCK_LIST_STORAGE, NO_STORAGE };
    
    class IMemoryStorage {
    public:
        virtual void initialize() = 0;
        virtual void* operator[](size_t index) = 0;
        virtual AllocationStorageType getType() const = 0;
        virtual bool canGrow() const = 0;
        virtual size_t capacity() const = 0;
        virtual size_t maxSize() const = 0;
        virtual size_t objectSize() const = 0;
        virtual size_t getRequestedStorageSize() const = 0;
        virtual size_t getStorageSize() const = 0;
        virtual size_t getStorageCount() const = 0;
        virtual size_t getStorageInitialCount() const = 0;
    };

    class FixedSizeStorage : public IMemoryStorage {
    public:
    
        FixedSizeStorage(size_t objectSize, size_t size) :
            mObjects(nullptr),
            mRequestedSize(size),
            mObjectSize(((objectSize + sizeof(void *)-1) / sizeof(void *)) * sizeof(void *)),
            mBlockSize(mObjectSize * (size/mObjectSize))
        {
            assert(objectSize <= size);
            assert(mObjectSize <= mBlockSize);
        }
        
        void initialize() override {
            mObjects = ::operator new(mBlockSize);
            std::cout << "new block of size: " << mBlockSize << std::endl;
            std::cout << "object size: " << mObjectSize << std::endl;
            std::cout << "allowed num objects per block: " << (mBlockSize / mObjectSize) << std::endl;
            std::memset(mObjects, 0, mBlockSize);
        }
        
        ~FixedSizeStorage() {
            if(mObjects)
                ::operator delete( mObjects );
        }
        
        void* operator[](size_t index) override {
            if(index >= (mBlockSize / mObjectSize)) throw std::bad_alloc();
            char * head = reinterpret_cast<char*>(mObjects);
            return reinterpret_cast<void*>(head + (index * mObjectSize));
        }
        
        size_t objectSize() const override { return mObjectSize; };
        size_t getRequestedStorageSize() const override { return mRequestedSize; }
        size_t getStorageSize() const override { return mBlockSize; }
        size_t getStorageCount() const override { return 1; }
        size_t getStorageInitialCount() const override { return 1; }
        bool canGrow() const override { return false; }
        size_t capacity() const override { return mBlockSize / mObjectSize; }
        size_t maxSize() const override { return mBlockSize / mObjectSize; }
        AllocationStorageType getType() const override { return FIXED_SIZE_STORAGE; }
        
    private:
        void* mObjects;
        const size_t mRequestedSize{0};
        const size_t mObjectSize{0};
        const size_t mBlockSize{0};
    };

    class BlockListStorage : public IMemoryStorage {
    public:
        
        BlockListStorage(size_t objectSize, size_t block_size, size_t num_blocks = 1) :
            mRequestedSize(block_size),
            mObjectSize(((objectSize + sizeof(void *)-1) / sizeof(void *)) * sizeof(void *)),
            mBlockSize(mObjectSize * (block_size/mObjectSize)),
            mInitalBlockCount(num_blocks)
        {}
        
        ~BlockListStorage(){
            mBlocks.clear();
        }
        
        void initialize() override {
            mBlocks = std::list<FixedSizeStorage>(mInitalBlockCount, FixedSizeStorage(mObjectSize, mBlockSize));
            for(auto & block: mBlocks){
                block.initialize();
            }
        }
        
        void* operator[](size_t index) override {
            size_t block = floor(index / (mBlockSize / mObjectSize));
            if(block == mBlocks.size()){
                std::cout << "exceeded block count, create a new one" << std::endl;
                mBlocks.emplace_back(mObjectSize, mBlockSize);
                mBlocks.back().initialize();
            }else if(block > mBlocks.size()){
                for(size_t i = 0; i < (block - mBlocks.size()); i++ ){
                    std::cout << "catching up..." << std::endl;
                    mBlocks.emplace_back(mObjectSize, mBlockSize);
                    mBlocks.back().initialize();
                }
            }
            auto it = mBlocks.begin();
            std::advance(it, block);
            return (*it)[index % (mBlockSize / mObjectSize)];
        }
        
        void freeBlock(size_t index){
            auto it = mBlocks.begin();
            std::advance(it,index);
            if(it != mBlocks.end()){
                mBlocks.erase(it);
            }
        }
        
        size_t objectSize() const override { return mObjectSize; };
        size_t getRequestedStorageSize() const override { return mRequestedSize; }
        size_t getStorageSize() const override { return mBlockSize; }
        size_t getStorageCount() const override { return mBlocks.size(); }
        size_t getStorageInitialCount() const override { return mInitalBlockCount; }
        AllocationStorageType getType() const override { return BLOCK_LIST_STORAGE; }
        bool canGrow() const override { return true; }
        size_t capacity() const override { return mBlocks.size() * (mBlockSize / mObjectSize); }
        size_t maxSize() const override { return mBlocks.size() * mBlockSize; }
        
    private:
        const size_t mRequestedSize{0};
        const size_t mObjectSize{0};
        const size_t mBlockSize{0};
        const size_t mInitalBlockCount{0};
        std::list<FixedSizeStorage> mBlocks;
    };
    
}//end namespace mediasystem
