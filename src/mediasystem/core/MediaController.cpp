//
//  MediaController.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "MediaController.h"
#include "ofMain.h"
#include "mediasystem/core/Scene.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/core/CoreEvents.h"

namespace mediasystem {
    
    MediaController::MediaController()
    {
        mEventManager.addDelegate<SceneChange>(EventDelegate::create<MediaController, &MediaController::onChangeScene>(this));
    }
    
    MediaController::~MediaController()
    {
    }
    
    void MediaController::init()
    {
        mEventManager.queueEvent(std::make_shared<InitSystem>());
        mEventManager.queueEvent(std::make_shared<PostInit>());
    }

    void MediaController::shutdown()
    {
        mEventManager.triggerEvent(std::make_shared<ShutdownSystem>());
        mComponentManager.clear();
        mSystemManager.clear();
        mScenes.clear();
        mEventManager.clear();
    }
    
    void MediaController::swapScenes(Scene*)
    {
        mCurrentScene->removeDelegate(Scene::Events::STOP, SceneEventDelegate::create<MediaController, &MediaController::swapScenes>(this));
        mCurrentScene = mNextScene;
        mNextScene = nullptr;
    }
    
    void MediaController::transition()
    {
        if(mCurrentScene){
            mCurrentScene->transitionOut();
            mCurrentScene->addDelegate(Scene::Events::STOP, SceneEventDelegate::create<MediaController, &MediaController::swapScenes>(this));
        }
        
        if(mNextScene)
            mNextScene->transitionIn();
    }
    
    void MediaController::onChangeScene(const IEventRef& sceneChange)
    {
        auto cast = std::static_pointer_cast<SceneChange>(sceneChange);
        mNextScene = cast->getNextScene();
        if(mNextScene){
            transition();
        }else{
            for (int i = 0; i < mScenes.size(); i++ ) {
                auto scene = mScenes[i];
                if (scene->getName().compare(cast->getNextSceneName()) == 0) {
                    mNextScene = scene;
                    transition();
                }
            }
            if(!mNextScene){
                MS_LOG_ERROR("dont have a scene by the name: " + cast->getNextSceneName());
                return nullptr;
            }
        }
    }
    
    void MediaController::update()
    {
        if(mCurrentScene)
            mCurrentScene->update();
        
        if(mNextScene)
            mNextScene->update();
        
        auto now = ofGetElapsedTimef();
        mEventManager.queueEvent(std::make_shared<UpdateEvent>(ofGetFrameNum(),now, now - mLastUpdateTime));
        mLastUpdateTime = now;
        mEventManager.update(); //everything happens here, except draw.
    }
    
    void MediaController::reset()
    {
        mEventManager.queueEvent(std::make_shared<ResetSystem>());
    }

}//end namespace mediasystem
