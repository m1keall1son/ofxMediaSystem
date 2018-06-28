#pragma once

#include <vector>
#include <memory>
#include <map>
#include "mediasystem/events/EventManager.h"
#include "mediasystem/events/SceneEvents.h"
#include "mediasystem/core/ComponentSystem.hpp"
#include "mediasystem/core/Scene.h"

namespace mediasystem {
    
    class Scene;

	class SceneManager {

	public:
        
		SceneManager();
		~SceneManager();

        void initScenes();
        void resetScenes();
        void shutdownScenes();
        void clear();
        
        void update();
        void draw();
        
        inline uint32_t getNumScenes()const { return mScenes.size(); }
        
        template<typename SceneType, typename...Args>
        std::shared_ptr<SceneType> createScene(Args&&...args)
        {
            static_assert(std::is_base_of<Scene,SceneType>::value, "SceneType must be derived from mediasystem::Scene");
            mScenes.emplace_back(std::make_shared<SceneType>(std::forward<Args>(args)...));
            mScenes.back()->addDelegate<SceneChange>(EventDelegate::create<SceneManager, &SceneManager::onChangeScene>(this));
            return std::static_pointer_cast<SceneType>(mScenes.back());
        }
        
        std::shared_ptr<Scene> createScene(const std::string& name, int eventDequeuTimeLimit = TimedQueue<IEventRef>::NO_TIME_LIMIT);
        
        void changeSceneTo(const std::string& scene);
        void changeSceneTo(std::shared_ptr<Scene> scene);
        void addScene(std::shared_ptr<Scene> scene);
        void destroyScene(const std::string& name);
        void destroyScene(const std::shared_ptr<Scene>& scene);
        
        std::shared_ptr<Scene> getCurrentScene() const { return mCurrentScene; }
        std::shared_ptr<Scene> getNextScene() const { return mNextScene; }
        std::shared_ptr<Scene> getScene(const std::string& name) const;
        std::vector<std::shared_ptr<Scene>>& getScenes(){ return mScenes; }

	protected:
        
        EventStatus onChangeScene(const IEventRef& sceneChange);

        void transition();
        EventStatus swapScenes(const IEventRef&);

        std::shared_ptr<Scene> mNextScene{nullptr};
        std::shared_ptr<Scene> mCurrentScene{nullptr};
		std::vector<std::shared_ptr<Scene>> mScenes;
        
	};

}//end namespace pf
