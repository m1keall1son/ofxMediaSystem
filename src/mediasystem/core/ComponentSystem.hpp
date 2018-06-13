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
#include "TypeID.hpp"

namespace mediasystem {
    
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
    
    using ComponentMap = std::unordered_map<size_t, std::shared_ptr<void>>;
    
    class ComponentManager {
    public:
            
        template<typename ComponentType, typename...Args>
        std::weak_ptr<ComponentType> createComponent(size_t entity_id, Args&&...args){
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
        std::weak_ptr<ComponentType> getComponent(size_t entity_id){
            auto found = mComponents[type_id<ComponentType>].find(entity_id);
            if(found != mComponents[type_id<ComponentType>].end()){
                auto cast = std::static_pointer_cast<ComponentType>(found->second);
                return cast;
            }else{
                return std::weak_ptr<ComponentType>();
            }
        }
        
        template<typename ComponentType>
        bool destroyComponent(size_t entity_id){
            auto found = mComponents[type_id<ComponentType>].find(entity_id);
            if(found != mComponents[type_id<ComponentType>].end()){
                mComponents[type_id<ComponentType>].erase(found);
                return true;
            }else{
                return false;
            }
        }
        
        bool destroyComponent(type_id_t type, size_t entity_id){
            auto& map = mComponents[type];
            
            for(auto& p : map){
                p;
            }
            
            auto found = map.find(entity_id);
            if(found != mComponents[type].end()){
                mComponents[type].erase(found);
                return true;
            }else{
                return false;
            }
        }
        
        template<typename ComponentType>
        ComponentMap& getComponents(){
            return mComponents[type_id<ComponentType>];
        }
        
        void clear(){
            mComponents.clear();
        }
        
    private:
        std::map<type_id_t, ComponentMap> mComponents;
    };
    
}//end namespace mediasystem


