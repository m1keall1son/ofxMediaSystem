//
//  SceneEvents.h
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once

#include <string>
#include "mediasystem/events/IEvent.h"
#include "mediasystem/util/TypeID.hpp"
#include "mediasystem/core/Handle.h"

namespace mediasystem {
    
    class Scene;
    class Entity;
    
    //sent each time a scene adds a new entity
    class NewEntity : public Event<NewEntity> {
    public:
        NewEntity(Handle<Entity>&& entity):mEntity(std::move(entity)){}
        NewEntity(const Handle<Entity>& entity):mEntity(entity){}
        inline Handle<Entity> getEntity(){ return mEntity; }
    private:
        Handle<Entity> mEntity;
    };
    
    //sent each time a scene destroys an existing entity
    class DestroyEntity : public Event<DestroyEntity> {
    public:
        DestroyEntity(Handle<Entity>&& entity):mEntity(std::move(entity)){}
        DestroyEntity(const Handle<Entity>& entity):mEntity(entity){}
        inline Handle<Entity> getEntity(){ return mEntity; }
    private:
        Handle<Entity> mEntity;
    };
    
    //sent each time an entity adds a component of a specific type
    template<typename ComponentType>
    class NewComponent : public Event<NewComponent<ComponentType>> {
    public:
        NewComponent(Handle<Entity> entity, Handle<ComponentType> comp):mEntity(std::move(entity)),mComponent(std::move(comp)){}
        inline type_id_t getComponentType(){ return type_id<ComponentType>; }
        Handle<ComponentType> getComponentHandle(){ return mComponent; }
        Handle<Entity> getEntityHandle(){ return mEntity; }
    private:
        Handle<Entity> mEntity;
        Handle<ComponentType> mComponent;
    };
    
    template<typename Self>
    class SceneEvent : public Event<Self> {
    public:
        SceneEvent(Scene& scene):mScene(scene){}
        virtual ~SceneEvent() = default;
        Scene& getScene(){ return mScene; }
    private:
        Scene& mScene;
    };

    //will cause the current scene to transition out and the scene designated in the message to
    //transition in
    class SceneChange : public SceneEvent<SceneChange> {
    public:
        enum Order { DRAW_OVER_PREVIOUS, DRAW_UNDER_PREVIOUS };
        SceneChange(Scene& current_scene, std::string next_sene, Order order = Order::DRAW_OVER_PREVIOUS);
        SceneChange(Scene& current_scene, StrongHandle<Scene> next_sene);
        const std::string& getNextSceneName()const{ return mNextSceneName; }
        StrongHandle<Scene> getNextScene(){ return mNextScene; }
        Order getDrawOrder(){ return mOrder; }
        void setDrawOverPrvious(){ mOrder = DRAW_OVER_PREVIOUS; }
        void setDrawUnderPrvious(){ mOrder = DRAW_UNDER_PREVIOUS; }
    private:
        StrongHandle<Scene> mNextScene{nullptr};
        std::string mNextSceneName;
        Order mOrder{DRAW_OVER_PREVIOUS};
    };
 
    class Init : public SceneEvent<Init> {
    public:
        Init(Scene& scene):SceneEvent<Init>(scene){}
    };
    
    class PostInit : public SceneEvent<PostInit> {
    public:
        PostInit(Scene& scene):SceneEvent<PostInit>(scene){}
    };
    
    class Shutdown : public SceneEvent<Shutdown> {
    public:
        Shutdown(Scene& scene):SceneEvent<Shutdown>(scene){}
    };
    
    class Reset : public SceneEvent<Reset> {
    public:
        Reset(Scene& scene):SceneEvent<Reset>(scene){}
    };
    
    class Start : public SceneEvent<Start> {
    public:
        Start(Scene& scene):SceneEvent<Start>(scene){}
    };
    
    class Stop : public SceneEvent<Stop>  {
    public:
        Stop(Scene& scene):SceneEvent<Stop>(scene){}
    };
    
    class Update : public SceneEvent<Update> {
    public:
        Update(Scene& scene, uint64_t elapsedFrames, double elapsedTime, double lastFrameTime );
        virtual ~Update() = default;
        inline uint64_t getElapsedFrames()const{ return mElapsedFrames; };
        inline double getElapsedTime()const{ return mElapsedTime; };
        inline double getLastFrameTime()const { return mLastFrameTime; }
    private:
        uint64_t mElapsedFrames;
        double mElapsedTime;
        double mLastFrameTime;
    };
    
    //draws all subscribers
    class Draw : public SceneEvent<Draw> {
    public:
        Draw(Scene& scene):SceneEvent<Draw>(scene){}
    };
    
    class TransitionInBegin : public SceneEvent<TransitionInBegin> {
    public:
        TransitionInBegin(Scene& scene):SceneEvent<TransitionInBegin>(scene){}
    };
    
    class TransitionInComplete : public SceneEvent<TransitionInComplete> {
    public:
        TransitionInComplete(Scene& scene):SceneEvent<TransitionInComplete>(scene){}
    };
    
    class TransitionOutBegin : public SceneEvent<TransitionOutBegin> {
    public:
        TransitionOutBegin(Scene& scene):SceneEvent<TransitionOutBegin>(scene){}
    };
    
    class TransitionOutComplete : public SceneEvent<TransitionOutComplete> {
    public:
        TransitionOutComplete(Scene& scene):SceneEvent<TransitionOutComplete>(scene){}
    };
    
    class TransitionUpdate : public SceneEvent<TransitionUpdate> {
    public:
        TransitionUpdate(Scene& scene):SceneEvent<TransitionUpdate>(scene){}
    };

}//end namespace mediasystem
