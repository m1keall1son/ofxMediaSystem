//
//  IAllocator.h
//  MemoryManagement
//
//  Created by Michael Allison on 4/6/18.
//

#pragma once

#include <stdint.h>
#include <cstddef>
#include <iostream>
#include "Storage.hpp"
#include "AllocationStrategies.hpp"
#include "AllocationMiddleware.hpp"

namespace mediasystem {
    
    //defaults to HEAP
    struct AllocationPolicyFormat {
        
        AllocationPolicyFormat(){
            for(auto& m : middleware){
                m = AllocationMiddlewareType::NO_MIDDLEWARE;
            }
        }

        AllocationPolicyFormat& defaultHeapStrategy(){ strategy = AllocationStrategyType::DEFAULT_HEAP; return *this; }
        AllocationPolicyFormat& unreclaimedPoolStrategy(){ strategy = AllocationStrategyType::UNRECLAIMED_POOL; return *this; }
        AllocationPolicyFormat& noStorage(){ storage = AllocationStorageType::NO_STORAGE; return *this; }
        AllocationPolicyFormat& fixedSizeStorage(size_t size){ storageSize = size; requestedStorageSize = size; storage = AllocationStorageType::FIXED_SIZE_STORAGE; return *this; }
        AllocationPolicyFormat& blockListStorage(size_t size, size_t initial_count = 1){
            storageSize = size;
            requestedStorageSize = size;
            storageInitialCount = initial_count;
            storage = AllocationStorageType::BLOCK_LIST_STORAGE;
            return *this;
        }

        AllocationStrategyType strategy{AllocationStrategyType::DEFAULT_HEAP};
        AllocationStorageType storage{AllocationStorageType::NO_STORAGE};
        size_t storageSize{0};
        size_t storageInitialCount{1};
        size_t requestedStorageSize{0};
        AllocationPolicyFormat& addConsoleLoggerMiddleware(){ middleware[AllocationMiddlewareType::CONSOLE_LOGGER] = AllocationMiddlewareType::CONSOLE_LOGGER; return *this; }
        std::array<AllocationMiddlewareType,AllocationMiddlewareType::NO_MIDDLEWARE> middleware;
        
    };
    
    class IAllocationPolicy {
    public:
        virtual void initialize() = 0;
        virtual void* allocate(size_t count) = 0;
        virtual void deallocate(void* ptr, size_t count) = 0;
        virtual AllocationStrategyType getStrategyType() const = 0;
        virtual AllocationStorageType getStorageType() const = 0;
        virtual size_t getRequestedStorageSize() const = 0;
        virtual size_t getStorageSize() const = 0;
        virtual size_t getStorageCount() const = 0;
        virtual size_t getStorageInitialCount() const = 0;
        virtual AllocationPolicyFormat getFormat() const = 0;
        virtual std::vector<AllocationMiddlewareType> getMiddlewareTypes() const = 0;
        virtual void addMiddleware( std::unique_ptr<IAllocaitonMiddleware>&& middleware ) = 0;
    };
    
    template<typename Strategy, typename Storage>
    class AllocationPolicy : public IAllocationPolicy {
    public:
        
        template<typename...Args>
        AllocationPolicy( Args&&...args ) :
            mStorage(std::forward<Args>(args)...)
        {}
        
        void initialize() override {
            mStrategy.initialize();
            mStorage.initialize();
            mInitialized = true;
        }
        
        void* allocate(size_t count) override
        {
            if(!mInitialized)
                initialize();
            auto ret = mStrategy.allocate( count, mStorage );
#if defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
            for(auto & middleware : mMiddlewares){
                if(middleware)
                    middleware->onAllocation(ret, count);
            }
#endif
            return ret;
        }
        
        void deallocate(void* ptr, size_t count) override
        {
            mStrategy.deallocate( ptr, count, mStorage );
#if defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
            for(auto & middleware : mMiddlewares){
                if(middleware)
                    middleware->onDeallocation(ptr, count);
            }
#endif
        }
        
        void addMiddleware( std::unique_ptr<IAllocaitonMiddleware>&& middleware )override{
            mMiddlewares[middleware->getType()] = std::move(middleware);
        }
        
        AllocationStrategyType getStrategyType() const override { return mStrategy.getType(); }
        AllocationStorageType getStorageType() const override { { return mStorage.getType(); } }
        size_t getRequestedStorageSize() const override { return mStorage.getRequestedStorageSize(); }
        size_t getStorageSize() const override { return mStorage.getStorageSize(); }
        size_t getStorageCount() const override { return mStorage.getStorageCount(); }
        size_t getStorageInitialCount() const override { return mStorage.getStorageInitialCount(); }
        std::vector<AllocationMiddlewareType> getMiddlewareTypes() const override {
            std::vector<AllocationMiddlewareType> ret;
            for(auto & middleware: mMiddlewares){
                if(middleware){
                    ret.push_back(middleware->getType());
                }else{
                    ret.push_back(AllocationMiddlewareType::NO_MIDDLEWARE);
                }
            }
            return ret;
        }
        
        AllocationPolicyFormat getFormat() const override {
            AllocationPolicyFormat fmt;
            fmt.strategy = mStrategy.getType();
            fmt.storage = mStorage.getType();
            fmt.storageSize = mStorage.getStorageSize();
            fmt.requestedStorageSize = mStorage.getRequestedStorageSize();
            fmt.storageInitialCount = mStorage.getStorageInitialCount();
            size_t i = 0;
            for(auto & middleware: mMiddlewares){
                if(middleware){
                    fmt.middleware[i] = middleware->getType();
                }else{
                    fmt.middleware[i] = AllocationMiddlewareType::NO_MIDDLEWARE;
                }
                ++i;
            }
            return fmt;
        }

        
    private:
        std::array<std::unique_ptr<IAllocaitonMiddleware>,AllocationMiddlewareType::NO_MIDDLEWARE> mMiddlewares;
        bool mInitialized{false};
        Storage mStorage;
        Strategy mStrategy;
    };
    
    template<typename T>
    class DefaultHeapAllocation : public IAllocationPolicy {
    public:
        
        void initialize() override {}
        
        void* allocate(size_t count) override
        {
            auto ret = ::operator new(count * sizeof(T), ::std::nothrow);
#if defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
            for(auto & middleware : mMiddlewares){
                if(middleware)
                    middleware->onAllocation(ret, count);
            }
#endif
            return ret;
        }
        
        void deallocate(void* ptr, size_t count) override
        {
            ::operator delete(ptr);
#if defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
            for(auto & middleware : mMiddlewares){
                if(middleware)
                    middleware->onDeallocation(ptr, count);
            }
#endif
        }
        
        void addMiddleware( std::unique_ptr<IAllocaitonMiddleware>&& middleware ) override {
            mMiddlewares[middleware->getType()] = std::move(middleware);
        }
        
        AllocationStrategyType getStrategyType() const override { return DEFAULT_HEAP; }
        AllocationStorageType getStorageType() const override { { return NO_STORAGE; } }
        size_t getStorageSize() const override { return 0; }
        size_t getRequestedStorageSize() const override { return 0; }
        size_t getStorageCount() const override { return 0; }
        size_t getStorageInitialCount() const override { return 0; }
        std::vector<AllocationMiddlewareType> getMiddlewareTypes() const override {
            std::vector<AllocationMiddlewareType> ret;
            for(auto & middleware: mMiddlewares){
                if(middleware){
                    ret.push_back(middleware->getType());
                }else{
                    ret.push_back(AllocationMiddlewareType::NO_MIDDLEWARE);
                }
            }
            return ret;
        }
        
        AllocationPolicyFormat getFormat() const override {
            AllocationPolicyFormat fmt; //use default
            size_t i = 0;
            for(auto & middleware: mMiddlewares){
                if(middleware){
                    fmt.middleware[i] = middleware->getType();
                }else{
                    fmt.middleware[i] = AllocationMiddlewareType::NO_MIDDLEWARE;
                }
                ++i;
            }
            return fmt;
        }
        
    private:
        std::array<std::unique_ptr<IAllocaitonMiddleware>,AllocationMiddlewareType::NO_MIDDLEWARE> mMiddlewares;
    };
    
    
}//end namspace mediasystem

inline std::ostream& operator<<(std::ostream& stream, const mediasystem::AllocationPolicyFormat& fmt) {
    stream << "[AllocationPolicyFormat]\n";
    switch(fmt.strategy){
        case mediasystem::UNRECLAIMED_POOL:{
            stream << "\tstrategy - UNRECLAIMED_POOL\n";
        }break;
        case mediasystem::DEFAULT_HEAP:{
            stream << "\tstrategy - DEFAULT_HEAP\n";
        }break;
    }
    switch(fmt.storage){
        case mediasystem::NO_STORAGE:{
            stream << "\tstorage - NO_STORAGE\n";
        }break;
        case mediasystem::BLOCK_LIST_STORAGE:{
            stream << "\tstorage - BLOCK_LIST_STORAGE\n";
            stream << "\t\trequested size - " << fmt.requestedStorageSize << "\n";
            stream << "\t\tactual size - " << fmt.storageSize << "\n";
            stream << "\t\tinitial count - " << fmt.storageInitialCount << "\n";
        }break;
        case mediasystem::FIXED_SIZE_STORAGE:{
            stream << "\tstorage - FIXED_SIZE_STORAGE\n";
            stream << "\t\trequested size - " << fmt.requestedStorageSize << "\n";
            stream << "\t\tactual size - " << fmt.storageSize << "\n";
            stream << "\t\tinitial count - " << fmt.storageInitialCount<<"\n";
        }break;
    }
#if defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
    for(auto & m : fmt.middleware){
        switch(m){
            case mediasystem::CONSOLE_LOGGER:{
                stream << "\tmiddleware: CONSOLE_LOGGER\n";
            }
            default: break;
        }
    }
#endif
    return stream;
}
