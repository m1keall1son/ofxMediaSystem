//
//  AllocationStrategies.hpp
//  Belron
//
//  Created by Michael Allison on 10/11/18.
//

#pragma once

#include <cstring>
#include <stdint.h>
#include <memory>
#include "Storage.hpp"

namespace mediasystem {
    
    enum AllocationStrategyType { DEFAULT_HEAP, UNRECLAIMED_POOL };
    
    class IAllocationStrategy {
    public:
        virtual void initialize() = 0;
        virtual void* allocate( size_t count, IMemoryStorage& storage ) = 0;
        virtual void deallocate( void* ptr, size_t count, IMemoryStorage& storage ) = 0;
        virtual bool canReclaim() const = 0;
        virtual AllocationStrategyType getType() const = 0;
    };
    
    class UnreclaimedPool : public IAllocationStrategy {
    public:
        
        AllocationStrategyType getType() const override { return UNRECLAIMED_POOL; }
        
        void initialize() override {}
        
        void* allocate( size_t count, IMemoryStorage& storage ) override {
            if(count == 1){
                void* ret;
                if(mFreeStore){
                    ret = mFreeStore;
                    auto next = *reinterpret_cast<void**>(mFreeStore);
                    mFreeStore = next;
                }else{
                    ret = storage[mLast++];
                }
                return ret;
            }else{
                return ::operator new(count * storage.objectSize(), ::std::nothrow);
            }
        }
        
        void deallocate(void* ptr, size_t count, IMemoryStorage& storage) override {
            if(count == 1){
                *reinterpret_cast<void**>(ptr) = mFreeStore;
                mFreeStore = ptr;
            }else{
                ::operator delete(ptr);
            }
        }
        
        bool canReclaim() const override { return false; }
    
    private:
        void* mFreeStore{nullptr};
        size_t mLast{0};
    };
    

}//end namespace mediasystem
