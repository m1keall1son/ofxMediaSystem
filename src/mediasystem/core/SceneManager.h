#pragma once

#include <vector>
#include <memory>
#include <map>
#include "mediasystem/events/EventManager.h"
#include "mediasystem/events/SceneEvents.h"
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
        
        void update(float elapsedTime = -1, size_t frame = std::numeric_limits<size_t>::max());
        void draw();
        
        inline uint32_t getNumScenes()const { return mScenes.size(); }
        
        template<typename SceneType, typename...Args>
        StrongHandle<SceneType> createScene(Args&&...args)
        {
            static_assert(std::is_base_of<Scene,SceneType>::value, "SceneType must be derived from mediasystem::Scene");
            mScenes.emplace_back(makeStrongHandle<SceneType>(std::forward<Args>(args)...));
            mScenes.back()->addDelegate<SceneChange>(EventDelegate::create<SceneManager, &SceneManager::onChangeScene>(this));
            return staticCast<SceneType>(mScenes.back());
        }
        
        StrongHandle<Scene> createScene(const std::string& name, AllocationManager&& allocator = AllocationManager());
        
        void changeSceneTo(const std::string& scene, SceneChange::Order drawOrder = SceneChange::Order::DRAW_OVER_PREVIOUS );
        void changeSceneTo(StrongHandle<Scene> scene, SceneChange::Order drawOrder = SceneChange::Order::DRAW_OVER_PREVIOUS );
        void addScene(StrongHandle<Scene> scene);
        void destroyScene(const std::string& name);
        void destroyScene(const StrongHandle<Scene>& scene);
        void resetFrameTime();
        
        StrongHandle<Scene> getCurrentScene() const { return mCurrentScene; }
        StrongHandle<Scene> getNextScene() const { return mNextScene; }
        StrongHandle<Scene> getScene(const std::string& name) const;
        std::vector<StrongHandle<Scene>>& getScenes(){ return mScenes; }

	protected:
        
        EventStatus onChangeScene(const IEventRef& sceneChange);

        void transition();
        EventStatus swapScenes(const IEventRef&);
        float mPrevTime{0.f};
        bool mSetTime{false};
        StrongHandle<Scene> mNextScene{nullptr};
        StrongHandle<Scene> mCurrentScene{nullptr};
        SceneChange::Order mDrawOrder;
        StrongHandle<Scene> mTop{nullptr};
        StrongHandle<Scene> mBottom{nullptr};
        std::vector<StrongHandle<Scene>> mScenes;
	};

}//end namespace pf
