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
#include "Handle.h"

namespace mediasystem {
    
    class SystemManager {
    public:
        
        template<typename SystemType, typename...Args>
        StrongHandle<SystemType> createSystem(Args&&...args){
            auto shared = makeStrongHandle<SystemType>(std::forward<Args>(args)...);
            auto generic = staticCast<void>(shared);
            auto it = mSystems.emplace(type_id<SystemType>,std::move(generic));
            if(it.second){
                return shared;
            }else{
                MS_LOG_ERROR("SystemManager: CANNOT CREATE SYSTEM");
                return nullptr;
            }
        }
        
        template<typename SystemType>
        StrongHandle<SystemType> getSystem(){
            auto found = mSystems.find(type_id<SystemType>);
            if(found != mSystems.end()){
                auto shared = staticCast<SystemType>(found->second);
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
        std::map<type_id_t, StrongHandle<void>> mSystems;
    };
    
    namespace detail {
        using GenericStrongHandle = StrongHandle<void>;
        using GenericStrongHandleAllocator = DynamicAllocator<std::pair<const size_t, GenericStrongHandle>>;
    }
    
    using GenericComponentMap = std::map<size_t, detail::GenericStrongHandle, std::less<size_t>, detail::GenericStrongHandleAllocator>;
    
    //adapter class
    template<typename ComponentType>
    class ComponentMap {
    public:
        
        ComponentMap() = default;
        
        class iterator {
        public:
            StrongHandle<ComponentType> next(){
                if(mIt == mEnd)
                    return nullptr;
                return staticCast<ComponentType>((*mIt++).second);
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
        Handle<ComponentType> create(size_t entity_id, Args&&...args){
            auto shared = allocateStrongHandle<ComponentType>(DynamicAllocator<ComponentType>(), std::forward<Args>(args)...);
            auto generic = staticCast<void>(shared);
            auto found = mComponents.find(type_id<ComponentType>);
            if(found != mComponents.end()){
                auto it = found->second.emplace( entity_id, std::move(generic) );
                if(it.second){
                    return shared;
                }else{
                    MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " COULD NOT CREATE COMPONENT");
                    return Handle<ComponentType>();
                }
            }else{
                auto it = mComponents.emplace(type_id<ComponentType>, GenericComponentMap(detail::GenericStrongHandleAllocator()));
                if(it.second){
                    auto res = it.first->second.emplace(entity_id, std::move(generic));
                    if(res.second){
                        return shared;
                    }else{
                        MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " COULD NOT CREATE COMPONENT");
                        return Handle<ComponentType>();
                    }
                }else{
                    MS_LOG_ERROR("ComponentManager: COULD NOT CREATE GENERIC COMPONENT MAP FOR COMPONENT TYPE");
                    return Handle<ComponentType>();
                }
            }
        }
        
        template<typename ComponentType>
        Handle<ComponentType> retrieve(size_t entity_id){
            auto found = mComponents[type_id<ComponentType>].find(entity_id);
            if(found != mComponents[type_id<ComponentType>].end()){
                auto cast = staticCast<ComponentType>(found->second);
                return cast;
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                return Handle<ComponentType>();
            }
        }
        
        Handle<void> retrieve(type_id_t type, size_t entity_id){
            auto found = mComponents[type].find(entity_id);
            if(found != mComponents[type].end()){
                return found->second;
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                return Handle<void>();
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


