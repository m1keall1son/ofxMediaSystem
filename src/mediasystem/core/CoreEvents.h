//
//  CoreEvents.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once

#include <string>
#include "mediasystem/events/IEvent.h"
#include "TypeId.hpp"

namespace mediasystem {
    
    class Scene;
    class Entity;
    using EntityHandle = std::weak_ptr<Entity>;
    
    //sent each time a scene adds a new entity
    class NewEntity : public Event<NewEntity> {
    public:
        NewEntity(EntityHandle&& entity):mEntity(std::move(entity)){}
        NewEntity(const EntityHandle& entity):mEntity(entity){}
        inline EntityHandle getEntity(){ return mEntity; }
    private:
        EntityHandle mEntity;
    };
    
    //sent each time a scene destroys an existing entity
    class DestroyEntity : public Event<DestroyEntity> {
    public:
        DestroyEntity(EntityHandle&& entity):mEntity(std::move(entity)){}
        DestroyEntity(const EntityHandle& entity):mEntity(entity){}
        inline EntityHandle getEntity(){ return mEntity; }
    private:
        EntityHandle mEntity;
    };
    
    //sent each time an entity adds a component of a specific type
    template<typename ComponentType>
    class NewComponent : public Event<NewComponent<ComponentType>> {
    public:
        NewComponent(std::weak_ptr<ComponentType>&& comp):mComponent(std::move(comp)){}
        NewComponent(const std::weak_ptr<ComponentType>& comp):mComponent(comp){}
        inline type_id_t getComponentType(){ return type_id<ComponentType>; }
        std::weak_ptr<ComponentType> getComponentHandle(){ return mComponent; }
    private:
        std::weak_ptr<ComponentType> mComponent;
    };
    
    //sent once on startup and once after reset
    struct InitSystem : Event<InitSystem> {};
    
    //triggered on exit from the app
    struct ShutdownSystem : Event<ShutdownSystem> {};
    
    //sent once following an InitSystem event
    struct PostInit : Event<PostInit> {};

    //will cause all things in the system to clear and reset state
    struct ResetSystem : Event<ResetSystem> {};

    //will cause the current scene to transition out and the scene designated in the message to
    //transition in
    class SceneChange : public Event<SceneChange> {
    public:
        SceneChange(std::string& scene);
        SceneChange(std::shared_ptr<Scene> scene);
        const std::string& getNextSceneName()const{ return mNextSceneName; }
        std::shared_ptr<Scene> getNextScene(){ return mNextScene; }
    private:
        std::shared_ptr<Scene> mNextScene{nullptr};
        std::string mNextSceneName;
    };
    
    //updates all subscribers
    class UpdateEvent : public Event<UpdateEvent> {
    public:
        UpdateEvent(uint64_t elapsedFrames, double elapsedTime, double lastFrameTime );
        virtual ~UpdateEvent() = default;
        inline uint64_t getElapsedFrames()const{ return mElapsedFrames; };
        inline double getElapsedTime()const{ return mElapsedTime; };
        inline double getLastFrameTime()const { return mLastFrameTime; }
    private:
        uint64_t mElapsedFrames;
        double mElapsedTime;
        double mLastFrameTime;
    };
    
    //draws all subscribers
    struct DrawEvent : Event<DrawEvent> {};

}//end namespace mediasystem
