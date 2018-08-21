//
//  MediaSystem.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#pragma once

#include <type_traits>
#include <unordered_map>

namespace mediasystem {
    
    using ManagedResourceKey = std::string;
    
    template<typename T>
    class Manager {
    public:
        
        using managed_type = T;
        
        Manager(size_t reserve = 0){
            mManaged.reserve(reserve);
        }
        
        template<typename ManageableType, typename...Args>
        std::weak_ptr<ManageableType> create(ManagedResourceKey key, Args&&...args){
            static_assert(std::is_base_of<T,ManageableType>::value, "Type passed to this create function must be the same or dervie from the managed type");
            auto it = mManaged.emplace(std::move(key),std::make_shared<ManageableType>(std::forward<Args>(args)...));
            if(it.second){
                return std::weak_ptr<ManageableType>(it.first->second);
            }
            return std::weak_ptr<ManageableType>();
        }
        
        template<typename ManageableType, typename Allocator, typename...Args>
        std::weak_ptr<ManageableType> allocate(ManagedResourceKey key, const Allocator& alloc, Args&&...args){
            static_assert(std::is_base_of<T,ManageableType>::value, "Type passed to this create function must be the same or dervie from the managed type");
            auto it = mManaged.emplace(std::move(key),std::allocate_shared<ManageableType>(alloc, std::forward<Args>(args)...));
            if(it.second){
                return std::weak_ptr<ManageableType>(it.first->second);
            }
            return std::weak_ptr<ManageableType>();
        }
        
        std::weak_ptr<T> retrieve(ManagedResourceKey id){
            auto found = mManaged.find(id);
            if(found != mManaged.end()){
                return std::weak_ptr<T>(found->second);
            }else{
                return std::weak_ptr<T>();
            }
        }
        
        bool remove(ManagedResourceKey key){
            return mManaged.erase(key) > 0;
        }
        
        void clear(){
            mManaged.clear();
        }
        
    protected:
        std::unordered_map<ManagedResourceKey, std::shared_ptr<T>>& getManagedItems(){ return mManaged; }
    private:
        std::unordered_map<ManagedResourceKey, std::shared_ptr<T>> mManaged;
    };
    
}//end namespace mediasystem

