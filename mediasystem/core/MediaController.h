#pragma once

#include <vector>
#include <memory>
#include <map>
#include "mediasystem/rendering/MediaRenderer.h"
#include "mediasystem/input/InputManager.h"

namespace mediasystem {
    
    class SceneBase;
    class IBehavior;
    class ViewBase;
    class InputComponent;

	class MediaController {

	public:
		
		MediaController();
		~MediaController();

		void init();
		void start(SceneBase* scene);
		void update();
		void draw();
        
        void allowDebugDrawing();
        void disableDebugDrawing();

		void nextScene(SceneBase* next);
		void addScene(std::unique_ptr<SceneBase>&& scene);
		SceneBase* getScene(const std::string& name) ;
		SceneBase* getCurrentScene();
		SceneBase* getNextScene();
		uint32_t getNumScenes();
		SceneBase* getScene(uint32_t index);
		void initializeScenes();
		void destroyScenes();
		void reset();
        void shutdown();

		template<typename Scene, typename...Args>
		Scene* createScene(const std::string& name, Args&&...args)
		{
			mScenes.emplace_back(new Scene(this, name, std::forward<Args>(args)...));
			auto scene = mScenes.back().get();
			return static_cast<Scene*>(scene);
		}
        
        template<typename Presenter, typename...Args>
        Presenter* createPresenter(Args&&...args){
           return mRenderer.createPresenter<Presenter>(std::forward<Args>(args)...);
        }
        
        //internal use
        void addToUpdateQueue( const std::weak_ptr<IBehavior>& handle );
        void addToRenderQueue( const std::weak_ptr<ViewBase>& handle );
        void registerInputComponent( const InputComponentHandle& handle );

	private:
        
        void initializeScene(SceneBase& scene);
    
		SceneBase* mNextScene;
		SceneBase* mCurrentScene;
		std::vector<std::unique_ptr<SceneBase>> mScenes;
        std::list<std::weak_ptr<IBehavior>> mUpdateable;
        MediaRenderer mRenderer;
        InputManager mInputManager;
        double mElspsedSeconds;
		bool mShouldAdvanceScenes;
        
	};

}//end namespace pf
