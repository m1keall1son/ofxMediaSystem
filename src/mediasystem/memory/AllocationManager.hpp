//
//  AllocationManager.hpp
//  Belron
//
//  Created by Michael Allison on 10/11/18.
//

#pragma once

#include "AllocationStrategies.hpp"
#include "Storage.hpp"
#include "AllocationMiddleware.hpp"
#include "AllocationPolicy.hpp"

namespace mediasystem {
    
    class AllocationManager {
    public:
        
        template<typename T>
        IAllocationPolicy* getPolicy(){
            IAllocationPolicy* ret = nullptr;
            auto found = mAllocaitonPolicies.find(type_id<T>);
            if( found != mAllocaitonPolicies.end()){
                return found->second.get();
            }
            return ret;
        }
        
        template<typename T>
        IAllocationPolicy* addPolicy(const AllocationPolicyFormat& fmt = AllocationPolicyFormat(), bool force = true){
            
            auto found = mAllocaitonPolicies.find(type_id<T>);
            if( found != mAllocaitonPolicies.end()){
                if(!force)
                    return found->second.get();
            }
            
            std::unique_ptr<IAllocationPolicy> policy;
            
            switch(fmt.strategy){
                case DEFAULT_HEAP:{
                    auto heapPolicy = std::unique_ptr<DefaultHeapAllocation<T>>( new DefaultHeapAllocation<T>);
                    policy = std::move(heapPolicy);
                }break;
                case UNRECLAIMED_POOL: {
                    switch(fmt.storage){
                        case FIXED_SIZE_STORAGE:{
                            auto pool = std::unique_ptr<AllocationPolicy<UnreclaimedPool,FixedSizeStorage>>( new AllocationPolicy<UnreclaimedPool,FixedSizeStorage>(sizeof(T), fmt.requestedStorageSize));
                            policy = std::move(pool);
                        }break;
                        case BLOCK_LIST_STORAGE:{
                            auto pool = std::unique_ptr<AllocationPolicy<UnreclaimedPool,BlockListStorage>>( new AllocationPolicy<UnreclaimedPool,BlockListStorage>(sizeof(T), fmt.requestedStorageSize, fmt.storageInitialCount));
                            policy = std::move(pool);
                        }break;
                        default:
                            throw std::runtime_error("An UNRECLAIMED_POOL allocator format MUST have a storage type.");
                            break;
                    }
                }break;
            }
#if defined(MS_ALLOW_ALLOCATION_MIDDLEWARE)
            for(auto & middleware : fmt.middleware){
                switch(middleware){
                    case CONSOLE_LOGGER:{
                        policy->addMiddleware( std::unique_ptr<AllocationConsoleLogger<T>>( new AllocationConsoleLogger<T>) );
                    }break;
                    default: continue;
                }
            }
#endif
            auto ret = policy.get();
            if( found != mAllocaitonPolicies.end()){
                if(force){
                    found->second = std::move(policy);
                }
            }else{
                mAllocaitonPolicies.emplace(type_id<T>, std::move(policy));
            }
            return ret;
        }
        
    private:
        std::map<type_id_t, std::unique_ptr<IAllocationPolicy>> mAllocaitonPolicies;
        //todo, could include initializers if they worked...
    };
    
}//end namespace mediasystem
