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
#include "mediasystem/util/TypeID.hpp"
#include "Scene.h"

namespace mediasystem {
    
    class Entity;
    using EntityRef = std::shared_ptr<Entity>;
    using EntityHandle = std::weak_ptr<Entity>;
    
    struct EntityGraph {
        EntityGraph(Entity& me);
        ~EntityGraph();
        
        Entity& self;
        EntityHandle parent;
        std::list<EntityHandle> children;
        
        void setParent(EntityHandle p, bool keepGlobalPosition = true);
        void clearParent(bool keepGlobalPosition = true);
        void addChild(EntityHandle child, bool keepGlobalPosition = true);
        void removeChild(EntityHandle child, bool keepGlobalPosition = true);
        
    };
    
    class Entity {
    public:
        
        explicit Entity(Scene& scene, uint64_t id);
        ~Entity() = default;
        
        //non copyable
        Entity(const Entity&) = delete;
        Entity& operator=(const Entity&) = delete;
        
        bool destroy();
        void clearComponents();
        
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
        std::shared_ptr<Component> getComponent() const{
            return mScene.getComponent<Component>(mId).lock();
        }
        
        template<typename Component>
        std::weak_ptr<Component> getComponentHandle() const{
            return mScene.getComponent<Component>(mId);
        }
        
        //graph component pass through
        void setParent(EntityHandle parent, bool bMaintainGlobalTransform = false);
        EntityHandle getParent() const;
        void clearParent(bool bMaintainGlobalTransform = false);
        void addChild(EntityHandle child);
        void removeChild(EntityHandle child);
        std::list<EntityHandle>& getChildren();

        //node component pass through
        void setPosition(float px, float py, float pz);
        void setPosition(const glm::vec3& p);
        glm::vec3 getPosition() const;

        void setGlobalPosition(float px, float py, float pz);
        void setGlobalPosition(const glm::vec3& p);
        glm::vec3 getGlobalPosition() const;

        void setOrientation(const glm::quat& q);
        void setOrientation(const glm::vec3& eulerAngles);
        glm::vec3 getOrientationEulerRad() const;
        glm::vec3 getOrientationEulerDeg() const;
        glm::quat getOrientationQuat() const;
        glm::vec3 getUpDir() const;
        glm::vec3 getLookAtDir()const;
        glm::vec3 getZAxis() const;
        glm::vec3 getYAxis() const;
        glm::vec3 getXAxis() const;

        void setGlobalOrientation(const glm::quat& q);
        glm::quat getGlobalOrientation() const;

        void setScale(float s);
        void setScale(float sx, float sy, float sz);
        void setScale(const glm::vec3& s);
        glm::vec3 getScale() const;
        glm::vec3 getGlobalScale() const;
        
        glm::mat4 getGlobalTransformMatrix() const;
        const glm::mat4& getLocalTransformMatrix() const;

    private:
        
        static std::map<type_id_t, size_t> sComponentIds;

        template<typename ComponentType>
        static size_t getId(){
            auto found = sComponentIds.find(type_id<ComponentType>);
            if(found != sComponentIds.end()){
                return found->second;
            }else{
                auto typeID = type_id<ComponentType>;
                auto flag = sNextComponentId++;
                sComponentIds.insert({typeID, flag});
                return flag;
            }
        }
        
        static type_id_t getType(size_t id){
            for(auto& compId : sComponentIds){
                if(compId.second == id)
                    return compId.first;
            }
            return nullptr;
        }
        
        static size_t sNextComponentId;
        std::bitset<32> mComponents;
        size_t mId{std::numeric_limits<size_t>::max()};
        Scene& mScene;
        friend Scene;
    };
    
}//end namespace mediasystem

