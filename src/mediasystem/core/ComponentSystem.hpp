//
//  ComponentSystem.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once
#include <stddef.h>
#include <memory>
#include <unordered_map>
#include <map>
#include "mediasystem/util/TypeID.hpp"

namespace mediasystem {
    
    template<typename T, typename U>
    std::weak_ptr<T> convertHandle( const std::weak_ptr<U>& generic ){
        if(auto locked = generic.lock()){
            return std::static_pointer_cast<T>(locked);
        }
        return std::weak_ptr<T>();
    }
    
    //todo system manager doesn't make much sense, shuold it return handles instead of shared ptrs?
    class SystemManager {
    public:
        
        template<typename SystemType, typename...Args>
        std::shared_ptr<SystemType> createSystem(Args&&...args){
            auto shared = std::make_shared<SystemType>(std::forward<Args>(args)...);
            auto generic = std::static_pointer_cast<void>(shared);
            auto it = mSystems.emplace(type_id<SystemType>,std::move(generic));
            if(it.second){
                return shared;
            }else{
                return nullptr;
            }
        }
        
        template<typename SystemType>
        std::shared_ptr<SystemType> getSystem(){
            auto found = mSystems.find(type_id<SystemType>);
            if(found != mSystems.end()){
                auto shared = std::static_pointer_cast<SystemType>(found->second);
                return std::move(shared);
            }else{
                return nullptr;
            }
        }
        
        template<typename SystemType>
        bool destroySystem(){
            auto found = mSystems.find(type_id<SystemType>);
            if(found != mSystems.end()){
                mSystems.erase(found);
                return true;
            }else{
                return false;
            }
        }
        
        void clear(){
            mSystems.clear();
        }
        
    private:
        std::map<type_id_t, std::shared_ptr<void>> mSystems;
    };
    
    using GenericComponentMap = std::unordered_map<size_t, std::shared_ptr<void>>;
    
    //adapter class
    template<typename ComponentType>
    class ComponentMap {
    public:
        
        ComponentMap() = default;
        
        class iterator {
        public:
            std::shared_ptr<ComponentType> next(){
                if(mIt == mEnd)
                    return nullptr;
                return std::static_pointer_cast<ComponentType>((*mIt++).second);
            }
        private:
            iterator() = default;
            iterator( std::unordered_map<size_t, std::shared_ptr<void>>::iterator begin, std::unordered_map<size_t, std::shared_ptr<void>>::iterator end ):mIt(begin),mEnd(){}
            
            std::unordered_map<size_t, std::shared_ptr<void>>::iterator mIt;
            std::unordered_map<size_t, std::shared_ptr<void>>::iterator mEnd;
            friend ComponentMap;
        };
        
        iterator iter(){ return mComponents ? iterator(mComponents->begin(), mComponents->end()) : iterator(); }
        size_t size() const { return mComponents ? mComponents->size() : 0; }
        bool empty() const { return mComponents ? mComponents->empty() : true; }
        
    private:
        ComponentMap(GenericComponentMap* components):mComponents(components){}
        GenericComponentMap* mComponents{nullptr};
        friend class ComponentManager;
    };
    
    class ComponentManager {
    public:
            
        template<typename ComponentType, typename...Args>
        std::weak_ptr<ComponentType> create(size_t entity_id, Args&&...args){
            auto shared = std::make_shared<ComponentType>(std::forward<Args>(args)...);
            auto generic = std::static_pointer_cast<void>(shared);
            auto it = mComponents[type_id<ComponentType>].emplace( entity_id, std::move(generic) );
            if(it.second){
                return shared;
            }else{
                return std::weak_ptr<ComponentType>();
            }
        }
        
        template<typename ComponentType>
        std::weak_ptr<ComponentType> retrieve(size_t entity_id){
            auto found = mComponents[type_id<ComponentType>].find(entity_id);
            if(found != mComponents[type_id<ComponentType>].end()){
                auto cast = std::static_pointer_cast<ComponentType>(found->second);
                return cast;
            }else{
                return std::weak_ptr<ComponentType>();
            }
        }
        
        std::weak_ptr<void> retrieve(type_id_t type, size_t entity_id){
            auto found = mComponents[type].find(entity_id);
            if(found != mComponents[type].end()){
                return found->second;
            }else{
                return std::weak_ptr<void>();
            }
        }
        
        template<typename ComponentType>
        bool destroy(size_t entity_id){
            auto found = mComponents[type_id<ComponentType>].find(entity_id);
            if(found != mComponents[type_id<ComponentType>].end()){
                mComponents[type_id<ComponentType>].erase(found);
                return true;
            }else{
                return false;
            }
        }
        
        bool destroy(type_id_t type, size_t entity_id){
            auto& map = mComponents[type];
            auto found = map.find(entity_id);
            if(found != mComponents[type].end()){
                mComponents[type].erase(found);
                return true;
            }else{
                return false;
            }
        }
        
        template<typename ComponentType>
        ComponentMap<ComponentType> getComponents(){
            return ComponentMap<ComponentType>(&mComponents[type_id<ComponentType>]);
        }
        
        void clear(){
            mComponents.clear();
        }
        
    private:
        std::map<type_id_t, GenericComponentMap> mComponents;
    };
    
}//end namespace mediasystem


