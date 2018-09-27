//
//  ComponentSystem.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once
#include <stddef.h>
#include <memory>
#include <map>
#include "mediasystem/util/TypeID.hpp"
#include "../util/Log.h"
#include "../memory/Memory.h"

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
                MS_LOG_ERROR("SystemManager: CANNOT CREATE SYSTEM");
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
                MS_LOG_ERROR("SystemManager: DOES NOT HAVE SYSTEM");
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
                MS_LOG_ERROR("SystemManager: DOES NOT HAVE SYSTEM");
                return false;
            }
        }
        
        void clear(){
            mSystems.clear();
        }
        
    private:
        std::map<type_id_t, std::shared_ptr<void>> mSystems;
    };
    
    namespace detail {
        using GenericHandle = std::shared_ptr<void>;
        using GenericHandleAllocator = DynamicAllocator<std::pair<const size_t, GenericHandle>>;
    }
    
    using GenericComponentMap = std::map<size_t, detail::GenericHandle, std::less<size_t>, detail::GenericHandleAllocator>;
    
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
            iterator( GenericComponentMap::iterator begin, GenericComponentMap::iterator end ):mIt(begin),mEnd(end){}
            GenericComponentMap::iterator mIt;
            GenericComponentMap::iterator mEnd;
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
            using ComponentAllocator = Allocator<ComponentType, PoolAllocator<ComponentType,BlockListStorage,1024>>; //1MB blocks
            auto shared = std::allocate_shared<ComponentType>(ComponentAllocator(), std::forward<Args>(args)...);
            auto generic = std::static_pointer_cast<void>(shared);
            auto found = mComponents.find(type_id<ComponentType>);
            if(found != mComponents.end()){
                auto it = found->second.emplace( entity_id, std::move(generic) );
                if(it.second){
                    return shared;
                }else{
                    MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " COULD NOT CREATE COMPONENT");
                    return std::weak_ptr<ComponentType>();
                }
            }else{
                auto it = mComponents.emplace(type_id<ComponentType>, GenericComponentMap(detail::GenericHandleAllocator()));
                if(it.second){
                    auto res = it.first->second.emplace(entity_id, std::move(generic));
                    if(res.second){
                        return shared;
                    }else{
                        MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " COULD NOT CREATE COMPONENT");
                        return std::weak_ptr<ComponentType>();
                    }
                }else{
                    MS_LOG_ERROR("ComponentManager: COULD NOT CREATE GENERIC COMPONENT MAP FOR COMPONENT TYPE");
                    return std::weak_ptr<ComponentType>();
                }
            }
        }
        
        template<typename ComponentType>
        std::weak_ptr<ComponentType> retrieve(size_t entity_id){
            auto found = mComponents[type_id<ComponentType>].find(entity_id);
            if(found != mComponents[type_id<ComponentType>].end()){
                auto cast = std::static_pointer_cast<ComponentType>(found->second);
                return cast;
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                return std::weak_ptr<ComponentType>();
            }
        }
        
        std::weak_ptr<void> retrieve(type_id_t type, size_t entity_id){
            auto found = mComponents[type].find(entity_id);
            if(found != mComponents[type].end()){
                return found->second;
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
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
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
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
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
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


