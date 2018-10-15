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
    
    template<typename Key, typename T>
    class Manager {
    public:
        
        using key_type = Key;
        using managed_type = T;
        
        Manager(size_t reserve = 0){
            mManaged.reserve(reserve);
        }
        
        template<typename ManageableType, typename...Args>
        Handle<ManageableType> create(Key key, Args&&...args){
            static_assert(std::is_base_of<T,ManageableType>::value, "Type passed to this create function must be the same or dervie from the managed type");
            auto it = mManaged.emplace(std::move(key),makeStrongHandle<ManageableType>(std::forward<Args>(args)...));
            if(it.second){
                return Handle<ManageableType>(it.first->second);
            }
            return Handle<ManageableType>();
        }
        
        template<typename ManageableType, typename Allocator, typename...Args>
        Handle<ManageableType> allocate(Key key, const Allocator& alloc, Args&&...args){
            static_assert(std::is_base_of<T,ManageableType>::value, "Type passed to this create function must be the same or dervie from the managed type");
            auto it = mManaged.emplace(std::move(key),allocateStrongHandle<ManageableType>(alloc, std::forward<Args>(args)...));
            if(it.second){
                return Handle<ManageableType>(it.first->second);
            }
            return Handle<ManageableType>();
        }
        
        Handle<T> retrieve(Key id){
            auto found = mManaged.find(id);
            if(found != mManaged.end()){
                return Handle<T>(found->second);
            }else{
                return Handle<T>();
            }
        }
        
        bool remove(Key key){
            return mManaged.erase(key) > 0;
        }
        
        void clear(){
            mManaged.clear();
        }
        
    protected:
        std::unordered_map<Key, StrongHandle<T>>& getManagedItems(){ return mManaged; }
    private:
        std::unordered_map<Key, StrongHandle<T>> mManaged;
    };
    
}//end namespace mediasystem

