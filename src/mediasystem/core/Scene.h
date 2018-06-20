#pragma once
#include <string>
#include <unordered_map>
#include "ofMain.h"
#include "mediasystem/events/EventManager.h"
#include "mediasystem/core/ComponentSystem.hpp"
#include "mediasystem/core/SceneEvents.h"

namespace mediasystem {
    
    class Entity;
    
    using EntityRef = std::shared_ptr<Entity>;
    using EntityHandle = std::weak_ptr<Entity>;
    
    class Scene : public EventManager {
	public:
        
        enum TransitionDir {
            TRANSITION_IN,
            TRANSITION_OUT
        };
        
        virtual ~Scene();
        
        Scene(const std::string& name, float transition_duration = 0.f);
        Scene(const std::string& name, float transition_in_duration, float transition_out_duration );
        
        //non copyable
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        
        virtual void init();
        virtual void postInit();
        
        virtual void transitionIn();
        virtual void transitionUpdate(){}
        virtual void transitionOut();
        
        virtual void update();
        virtual void draw();
        
        virtual void shutdown();
        virtual void reset();
		
        inline const std::string& getName() const { return mName; }
        
        virtual EntityHandle createEntity();
        virtual bool destroyEntity(size_t id);
        virtual bool destroyEntity(EntityHandle handle);
        virtual EntityHandle getEntity(size_t id);

        template<typename SystemType, typename...Args>
        std::shared_ptr<SystemType> createSystem(Args&&...args)
        {
            return mSystemManager.createSystem<SystemType>(std::forward<Args>(args)...);
        }
        
        template<typename SystemType>
        std::shared_ptr<SystemType> getSystem()
        {
            return mSystemManager.getSystem<SystemType>();
        }
        
        template<typename SystemType>
        bool destroySystem()
        {
            return mSystemManager.destroySystem<SystemType>();
        }
        
        template<typename Component, typename...Args>
        std::weak_ptr<Component> createComponent(size_t entity_id, Args&&...args){
            auto ret = mComponentManager.create<Component>(entity_id, std::forward<Args>(args)...);
            queueEvent<NewComponent<Component>>(ret);
            return ret;
        }
        
        template<typename Component>
        bool destroyComponent(size_t entity_id){
            return mComponentManager.destroy<Component>(entity_id);
        }
        
        template<typename Component>
        std::weak_ptr<Component> getComponent(size_t entity_id){
            return mComponentManager.retrieve<Component>(entity_id);
        }
        
        virtual std::weak_ptr<void> getComponent(type_id_t type, size_t entity_id);
        virtual bool destroyComponent(type_id_t type, size_t entity_id);

        inline bool hasStarted() const { return mHasStarted; }
        
        inline bool isTransitioning() const { return mIsTransitioning; }
        inline TransitionDir getTransitionDirection() const { return mTransitionDirection; }
        float getTransitionDuration(TransitionDir direction) const;
        void setTransitionDuration(TransitionDir direction, float duration);
        float getPercentTransitionComplete() const;
        
	protected:
        
        virtual void start();
        virtual void stop();
        
        virtual void transitionInComplete();
        virtual void transitionOutComplete();
        
		std::string	mName;
        std::unordered_map<size_t, EntityRef> mEntities;

	private:
        TransitionDir mTransitionDirection{TRANSITION_IN};
        bool mIsTransitioning{false};
        float mTransitionInDuration{0.f};
        float mTransitionOutDuration{0.f};
        float mTransitionStart{0.f};
        float mLastUpdateTime{0.f};
        
        bool mHasStarted{false};
        ComponentManager mComponentManager;
        SystemManager mSystemManager;
	};

}//end namespace mediasystem
