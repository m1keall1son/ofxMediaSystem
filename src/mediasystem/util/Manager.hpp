//
//  MediaSystem.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#pragma once

#include <type_traits>
#include <unordered_map>
#include "IMedia.h"

namespace mediasystem {
    
    using ManagedResourceId = uint16_t;
    
    template<typename T>
    class Manager {
    public:
        
        using managed_type = T;
        
        Manager(size_t reserve = 0){
            mManaged.reserve(reserve);
        }
        
        template<typename ManageableType, typename...Args>
        std::pair<ManagedResourceId,std::shared_ptr<ManageableType>> create(Args&&...args){
            static_assert(std::is_base_of<T,ManageableType>::value, "Type passed to this create function must be the same or dervie from the managed type");
            auto id = sNextId++;
            auto ret = std::make_pair(id,std::make_shared<ManageableType>(std::forward<Args>(args)...));
            mManaged.insert(ret);
            return ret;
        }
        
        template<typename ManageableType, typename Allocator, typename...Args>
        std::pair<ManagedResourceId,std::shared_ptr<ManageableType>> allocate(const Allocator& alloc, Args&&...args){
            static_assert(std::is_base_of<T,ManageableType>::value, "Type passed to this create function must be the same or dervie from the managed type");
            auto id = sNextId++;
            auto ret = std::make_pair(id,std::allocate_shared<ManageableType>(alloc, std::forward<Args>(args)...));
            mManaged.insert(ret);
            return ret;
        }
        
        std::weak_ptr<T> retrieve(ManagedResourceId id){
            auto found = mManaged.find(id);
            if(found != mManaged.end()){
                return std::weak_ptr<T>(found->second);
            }else{
                return std::weak_ptr<T>();
            }
        }
        
        void remove(ManagedResourceId id){
            mManaged.erase(id);
        }
        
        void clear(){
            mManaged.clear();
        }
        
    protected:
        
        std::unordered_map<ManagedResourceId, std::shared_ptr<T>>& getManagedItems(){ return mManaged; }
        
    private:
        
        std::unordered_map<ManagedResourceId, std::shared_ptr<T>> mManaged;
        static size_t sNextId;
        
    };
    
}//end namespace mediasystem

template<typename ManagedType>
size_t mediasystem::Manager<ManagedType>::sNextId = 0;
