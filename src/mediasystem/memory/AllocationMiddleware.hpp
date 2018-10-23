//
//  AllocaitonMiddleware.hpp
//  Belron
//
//  Created by Michael Allison on 10/11/18.
//

#pragma once
#include "ofMain.h"

namespace mediasystem {
    
    enum AllocationMiddlewareType { CONSOLE_LOGGER, NO_MIDDLEWARE };
    
    class IAllocaitonMiddleware {
    public:
        virtual void onAllocation(void* allocatedPtr, size_t count) = 0;
        virtual void onDeallocation(void* deallocatedPtr, size_t count) = 0;
        virtual AllocationMiddlewareType getType() const = 0;
    };

    template< typename T >
    class AllocationConsoleLogger : public IAllocaitonMiddleware {
    public:
        void onAllocation(void* allocatedPtr, size_t count) override
        {
            ofLogVerbose("Memory") << "Allocation: " << typeid(T).name() << "\n"
            << "\tbytes: " << count * sizeof(T) << "\n"
            << "\tlocation: " << allocatedPtr << "\n"
            << "\tT size: " << sizeof(T) << "\n"
            << "\tT count: " << count << "\n"
            << "\ttotal allocations of T: " << ++mAllocationsCount << "\n"
            << "\tcurrent allocations of T: " << ++mCurrentAllocationsCount;
        }
        
        void onDeallocation(void* ptr, size_t count) override
        {
            ofLogVerbose("Memory")  << "Deallocation: " << typeid(T).name() << "\n"
            << "\tbytes: " << count * sizeof(T) << "\n"
            << "\tlocation: " << ptr << "\n"
            << "\tT size: " << sizeof(T) << "\n"
            << "\tT count: " << count << "\n"
            << "\ttotal deallocations of T: " << ++mDeallocationsCount << "\n"
            << "\tcurrent allocations of T: " << --mCurrentAllocationsCount;
        }
        
        AllocationMiddlewareType getType() const override { return CONSOLE_LOGGER; }
        
    private:
        size_t mAllocationsCount{0};
        size_t mCurrentAllocationsCount{0};
        size_t mDeallocationsCount{0};
    };
        
}//end namespace mediasystem
