//
//  MediaController.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "MediaController.h"
#include "mediasystem/behaviors/ViewBase.h"
#include "mediasystem/core/SceneBase.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/util/ThreadPool.h"

namespace mediasystem {
    
    MediaController::MediaController() : mShouldAdvanceScenes(false)
    {
    }
    
    MediaController::~MediaController()
    {
        
    }
    
    void MediaController::init()
    {
        GlobalTime::init();
        //ThreadPoolManager::init();
        //ThreadPoolManager::get()->run();
        mInputManager.connect();
    }
    
    void MediaController::shutdown()
    {
        mInputManager.disconnect();
        reset();
        //ThreadPoolManager::get()->shutdown();
    }
    
    void MediaController::allowDebugDrawing()
    {
        mRenderer.setDebugDraw(true);
    }
    
    void MediaController::disableDebugDrawing()
    {
        mRenderer.setDebugDraw(false);
    }
    
    void MediaController::start(SceneBase* scene)
    {
        mNextScene = nullptr;
        mCurrentScene = scene;
        for(auto& scene: mScenes){
            scene->stop();
        }
        mCurrentScene->_start();
    }
    
    void MediaController::update()
    {
        auto tm = GlobalTime::get();
        tm->tick();
        
        if (mShouldAdvanceScenes) {
            mCurrentScene->stop();
            if(!mNextScene->isInit())
                initializeScene(*mNextScene);
            mNextScene->_start();
            mShouldAdvanceScenes = false;
            mCurrentScene = mNextScene;
            mNextScene = nullptr;
        }
        
        auto it = mUpdateable.begin();
        auto end = mUpdateable.end();
        while(it != end){
            if(auto behavior = (*it).lock()){
                if(behavior->isEnabled()){
                    behavior->update();
                }
                ++it;
            }else{
                it = mUpdateable.erase(it);
            }
        }
        
        mCurrentScene->update();
    }
    
    void MediaController::draw()
    {
        mRenderer.draw();
        mInputManager.update();
    }
    
    void MediaController::nextScene(SceneBase* next)
    {
        mNextScene = next;
        mShouldAdvanceScenes = true;
    }
    
    void MediaController::addScene(std::unique_ptr<SceneBase>&& scene)
    {
        mScenes.emplace_back(std::move(scene));
    }
    
    SceneBase * MediaController::getScene(const std::string & name)
    {
        SceneBase* found;
        for (int i = 0; i < mScenes.size(); i++ ) {
            auto scene = mScenes[i].get();
            if (scene->getName().compare(name) == 0) {
                found = scene;
                return found;
            }
        }
        
        if(!found){
            MS_LOG_ERROR("dont have a scene by the name: " + name);
            return nullptr;
        }
        else {
            return found;
        }
    }
    
    SceneBase * MediaController::getCurrentScene()
    {
        return mCurrentScene;
    }
    
    SceneBase * MediaController::getNextScene()
    {
        return mNextScene;
    }
    
    uint32_t MediaController::getNumScenes()
    {
        return mScenes.size();
    }
    
    SceneBase * MediaController::getScene(uint32_t index)
    {
        assert(index < mScenes.size());
        return mScenes[index].get();
    }
    
    void MediaController::initializeScenes()
    {
        for ( auto & scene : mScenes ) {
            initializeScene(*scene);
        }
    }
    
    void MediaController::initializeScene(SceneBase& scene)
    {
        scene.init();
        scene.initBehaviors();
        scene.setup();
    }
    
    void MediaController::destroyScenes()
    {
        mScenes.clear();
        mCurrentScene = nullptr;
        mNextScene = nullptr;
    }
    
    void MediaController::reset()
    {
        destroyScenes();
        mRenderer.clear();
        mUpdateable.clear();
    }
    
    void MediaController::addToRenderQueue( const ViewBaseHandle& handle )
    {
        auto b = handle.lock();
        mRenderer.insert(b->getLayer(), handle);
    }
    
    void MediaController::addToUpdateQueue( const BehaviorHandle& handle )
    {
        mUpdateable.push_back(handle);
    }
    
    void MediaController::registerInputComponent( const InputComponentHandle& handle )
    {
        mInputManager.registerInputComponent(handle);
    }

}//end namespace mediasystem
