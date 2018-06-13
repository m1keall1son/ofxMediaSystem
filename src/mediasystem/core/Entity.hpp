//
//  Entity.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once
#include <bitset>
#include <memory>
#include <numeric>
#include "TypeID.hpp"
#include "Scene.h"

namespace mediasystem {
    
    class Entity;
    using EntityRef = std::shared_ptr<Entity>;
    using EntityHandle = std::weak_ptr<Entity>;
    
    class Entity {
    public:
        
        ~Entity() = default;
        
        void destroy(){
            if(isValid()){
                for(int i = 0; i < mComponents.size(); i++){
                    if(mComponents[i]){
                        if(auto compType = getType(i)){
                            mScene.destroyComponent(compType, mId);
                        }
                    }
                }
                mComponents.reset();
                mScene.destroyEntity(mId);
                mId = std::numeric_limits<size_t>::max();
            }
        }
        
        inline bool isValid(){ return mId != std::numeric_limits<size_t>::max(); }
        inline size_t getId() const { return mId; }
        inline Scene& getScene(){ return mScene; }
        
        //convenience functions for working with components
        
        template<typename Component, typename...Args>
        std::weak_ptr<Component> createComponent(Args&&...args){
            
            auto compId = getId<Component>();
            
            if(mComponents[compId]){
                mScene.destroyComponent<Component>(mId);
            }
            
            auto ret = mScene.createComponent<Component>(mId, std::forward<Args>(args)...);

            if(!ret.expired()){
                mComponents[compId] = true;
            }
            return ret;
        }
        
        template<typename Component>
        bool destroyComponent(){
            auto compId = getId<Component>();
            if(mComponents[compId]){
                auto ret = mScene.destroyComponent<Component>(mId);
                if(ret){
                    mComponents[compId] = false;
                }
                return ret;
            }else{
                return false;
            }
        }
        
        template<typename Component>
        bool hasComponent(){
            return mComponents[getId<Component>()];
        }
        
        template<typename Component>
        std::weak_ptr<Component> getComponent(){
            return mScene.getComponent<Component>(mId);
        }
        
    private:
        
        static std::map<type_id_t, size_t> sComponentIds;

        template<typename ComponentType>
        static size_t getId(){
            auto found = sComponentIds.find(type_id<ComponentType>);
            if(found != sComponentIds.end()){
                return found->second;
            }else{
                auto flag = sNextComponentId++;
                sComponentIds.insert({type_id<ComponentType>, flag});
                return flag;
            }
        }
        
        static type_id_t getType(size_t id){
            for(auto& compId : sComponentIds){
                if(compId.second == id)
                    return compId.first;
            }
        }
        
        explicit Entity(Scene& scene, uint64_t id):
            mScene(scene),
            mId(id)
        {
            mComponents.reset();
        }
        static size_t sNextComponentId;
        std::bitset<32> mComponents;
        size_t mId{std::numeric_limits<size_t>::max()};
        Scene& mScene;
        friend Scene;
    };
    
}//end namespace mediasystem

inline std::map<mediasystem::type_id_t, size_t> mediasystem::Entity::sComponentIds = {};
inline size_t mediasystem::Entity::sNextComponentId = 0;

