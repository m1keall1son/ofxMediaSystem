#pragma once
#include <string>
#include <unordered_map>
#include "ofMain.h"
#include "mediasystem/events/MultiCastDelegate.h"
#include "MediaController.h"
#include "TypeId.hpp"

namespace mediasystem {
    
    class Entity;
    
    using EntityRef = std::shared_ptr<Entity>;
    using EntityHandle = std::weak_ptr<Entity>;
    class Scene;

    using SceneEvent = SA::multicast_delegate<void(Scene*)>;
    using SceneEventDelegate = SA::delegate<void(Scene*)>;

	class Scene {
	public:
        
        enum Transition {
            TRANSITION_IN,
            TRANSITION_OUT
        };
        
        enum Events {
            INIT,
            POST_INIT,
            START,
            STOP,
            UPDATE,
            TRANSITION_IN_BEGIN,
            TRANSITION_IN_END,
            TRANSITION_OUT_BEGIN,
            TRANSITION_OUT_END,
            TRANSITION_UPDATE
        };
        
        virtual ~Scene();
		
        inline const std::string& getName() const { return mName; }
        
        EntityHandle createEntity();
        bool destroyEntity(size_t id);
        bool destroyEntity(EntityHandle handle);
        EntityHandle getEntity(size_t id);

        template<typename Component, typename...Args>
        std::weak_ptr<Component> createComponent(Args&&...args){
            return mController.createComponent<Component>(std::forward<Args>(args)...);
        }
        
        template<typename Component>
        bool destroyComponent(size_t entity_id){
            return mController.destroyComponent<Component>(entity_id);
        }
        
        template<typename Component>
        std::weak_ptr<Component> getComponent(size_t entity_id){
            return mController.getComponent<Component>(entity_id);
        }
        
        bool destroyComponent(type_id_t type, size_t entity_id);

        inline bool hasStarted() const { return mHasStarted; }
        
        inline bool isTransitioning() const { return mIsTransitioning; }
        inline Transition getTransitionDirection() const { return mTransitionDirection; }
        float getTransitionDuration(Transition direction) const;
        void setTransitionDuration(Transition direction, float duration);
        float getPercentTransitionComplete() const;
        
        inline MediaController& getController(){ return mController; }

        void addDelegate(Scene::Events event, SceneEventDelegate delegate);
        void removeDelegate(Scene::Events event, SceneEventDelegate delegate);
        
	protected:
        
        virtual void init();
        virtual void postInit();
        virtual void update();
        
        virtual void transitionIn();
        virtual void transitionOut();
        
        virtual void transitionInComplete();
        virtual void transitionOutComplete();

        virtual void start();
        virtual void stop();
        
        virtual void shutdown();

		std::string	mName;
        std::unordered_map<size_t, EntityRef> mEntities;
        float mTransitionInDuration{0.f};
        float mTransitionOutDuration{0.f};
        float mTransitionStart{0.f};

	private:
        
        void onInit(const IEventRef& event);
        void onPostInit(const IEventRef& event);
        void onShutdown(const IEventRef& event);
        
        Scene(MediaController& context, const std::string& name, float transition_duration = 0.f);
        Scene(MediaController& context, const std::string& name, float transition_in_duration, float transition_out_duration );

        std::map<Scene::Events,SceneEvent> mEvents;
        
        bool mIsTransitioning{false};
        bool mHasStarted{false};
		MediaController& mController;
        Transition mTransitionDirection{TRANSITION_IN};
        
        friend MediaController;
	};

}//end namespace mediasystem
