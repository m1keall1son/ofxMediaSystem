#pragma once

#include <vector>
#include <memory>
#include <map>
#include "mediasystem/events/EventManager.h"
#include "mediasystem/core/CoreEvents.h"
#include "mediasystem/core/ComponentSystem.hpp"

namespace mediasystem {
    
    class Scene;

	class MediaController {

	public:
        
		MediaController();
		virtual ~MediaController();

		virtual void init();
		virtual void update();
		virtual void reset();
        virtual void shutdown();
        
        inline uint32_t getNumScenes()const { return mScenes.size(); }
        
        template<typename SceneType, typename...Args>
        std::shared_ptr<SceneType> createScene(const std::string& name, Args&&...args)
        {
            static_assert(std::is_base_of<Scene,SceneType>::value, "SceneType must be derived from mediasystem::Scene");
            mScenes.emplace_back(new SceneType(*this, name, std::forward<Args>(args)...));
            return mScenes.back();
        }
        
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
            auto ret = mComponentManager.createComponent<Component>(entity_id, std::forward<Args>(args)...);
            mEventManager.queueEvent<NewComponent<Component>>(ret);
            return ret;
        }
        
        template<typename Component>
        bool destroyComponent(size_t entity_id){
            return mComponentManager.destroyComponent<Component>(entity_id);            
        }
        
        inline bool destroyComponent(type_id_t type, size_t entity_id){
            return mComponentManager.destroyComponent(type, entity_id);
        }
        
        template<typename Component>
        std::weak_ptr<Component> getComponent(size_t entity_id){
            return mComponentManager.getComponent<Component>(entity_id);
        }
        
        inline EventManager& getEventManager(){ return mEventManager; }
 
	protected:
        
        void onChangeScene(const IEventRef& sceneChange);
        
        void transition();
        void swapScenes(Scene*);

        std::shared_ptr<Scene> mNextScene{nullptr};
        std::shared_ptr<Scene> mCurrentScene{nullptr};
		std::vector<std::shared_ptr<Scene>> mScenes;
        ComponentManager mComponentManager;
        SystemManager mSystemManager;
        EventManager mEventManager;
        float mLastUpdateTime{0.f};
        
	};

}//end namespace pf
