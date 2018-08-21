//
//  SceneManager.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "SceneManager.h"
#include "ofMain.h"
#include "mediasystem/core/Scene.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/events/GlobalEvents.h"

namespace mediasystem {
    
    SceneManager::SceneManager()
    {}
    
    SceneManager::~SceneManager()
    {}

    void SceneManager::shutdownScenes()
    {
        for(auto & scene : mScenes){
            scene->notifyShutdown();
        }
    }
    
    void SceneManager::resetScenes()
    {
        for(auto & scene : mScenes){
            scene->notifyReset();
        }
    }
    
    void SceneManager::initScenes()
    {
        for(auto & scene : mScenes){
            scene->notifyInit();
        }
        for(auto & scene : mScenes){
            scene->notifyPostInit();
        }
    }
    
    std::shared_ptr<Scene> SceneManager::createScene(const std::string& name, int eventDequeuTimeLimit)
    {
        auto scene = std::make_shared<Scene>(name, eventDequeuTimeLimit);
        addScene(scene);
        return scene;
    }
    
    void SceneManager::changeSceneTo(const std::string& nextScene)
    {
        mNextScene = nullptr;
        mNextScene = getScene(nextScene);
        if(!mNextScene){
            MS_LOG_ERROR("dont have a scene by the name: " + nextScene);
        }else{
            transition();
        }
    }
    
    void SceneManager::changeSceneTo(std::shared_ptr<Scene> scene)
    {
        mNextScene = scene;
        transition();
    }
    
    void SceneManager::addScene(std::shared_ptr<Scene> scene)
    {
        scene->addDelegate<SceneChange>(EventDelegate::create<SceneManager, &SceneManager::onChangeScene>(this));
        mScenes.emplace_back(std::move(scene));
    }
    
    EventStatus SceneManager::swapScenes(const IEventRef&)
    {
        MS_LOG_VERBOSE("Transitioning complete! swapping current and next");
        mCurrentScene = mNextScene;
        mNextScene = nullptr;
        return EventStatus::REMOVE_THIS_DELEGATE;
    }
    
    void SceneManager::transition()
    {
        if(mCurrentScene){
            if(mNextScene){
                mNextScene->mPreviousScene = mCurrentScene->getName();
                mNextScene->notifyTransitionIn();
                MS_LOG_VERBOSE("Transitioning to scene: " + mNextScene->getName());
            }
            
            MS_LOG_VERBOSE("Transitioning from scene: " + mCurrentScene->getName());
            mCurrentScene->notifyTransitionOut();
            if(mCurrentScene->isTransitioning()){
                mCurrentScene->addDelegate<Stop>(EventDelegate::create<SceneManager, &SceneManager::swapScenes>(this));
            }else{
                mCurrentScene = mNextScene;
                mNextScene = nullptr;
            }
            
        }else{
            mCurrentScene = mNextScene;
            mNextScene = nullptr;
            mCurrentScene->notifyTransitionIn();
            MS_LOG_VERBOSE("Changing scene to: " + mCurrentScene->getName());
        }
    }
    
    std::shared_ptr<Scene> SceneManager::getScene(const std::string& name) const
    {
        for (size_t i = 0; i < mScenes.size(); i++ ) {
            auto scene = mScenes[i];
            if (scene->getName().compare(name) == 0) {
                return scene;
            }
        }
        MS_LOG_ERROR("dont have a scene by the name: " + name);
        return nullptr;
    }
    
    EventStatus SceneManager::onChangeScene(const IEventRef& sceneChange)
    {
//        if(mCurrentScene && mCurrentScene->isTransitioning()){
//            return EventStatus::DEFER_EVENT;
//        }
        
        MS_LOG_VERBOSE("Received a scene change request...");
        
        auto cast = std::static_pointer_cast<SceneChange>(sceneChange);
        mNextScene = cast->getNextScene();
        if(mNextScene){
            transition();
        }else{
            changeSceneTo(cast->getNextSceneName());
        }
        return EventStatus::SUCCESS;
    }
    
    void SceneManager::resetFrameTime()
    {
        mPrevTime = 0.f;
        mSetTime = false;
    }
    
    void SceneManager::update(float elapsedTime, size_t frame)
    {
        float time = 0.f;
        if(elapsedTime == -1){
            time = ofGetElapsedTimef();
        }else{
            time = elapsedTime;
        }
        
        auto framenum = ofGetFrameNum();
        if(frame != std::numeric_limits<size_t>::max()){
            framenum = frame;
        }
        
        if(!mSetTime){
            mPrevTime = time;
            mSetTime = true;
        }
        
        auto dt = time - mPrevTime;
        
        if(mCurrentScene)
            mCurrentScene->notifyUpdate(framenum, time, dt);
        
        if(mNextScene)
            mNextScene->notifyUpdate(framenum, time, dt);
        
        mPrevTime = time;
    }
    
    void SceneManager::draw()
    {
        if(mCurrentScene)
            mCurrentScene->notifyDraw();
        
        if(mNextScene)
            mNextScene->notifyDraw();
    }

    void SceneManager::destroyScene(const std::string& name)
    {
        auto found = std::find_if(mScenes.begin(), mScenes.end(),[&name](const std::shared_ptr<Scene>& scene){
            return scene->getName() == name;
        });
        if(found != mScenes.end()){
            mScenes.erase(found);
        }else{
            MS_LOG_ERROR("dont have a scene by the name: " + name);
        }
    }
    
    void SceneManager::destroyScene(const std::shared_ptr<Scene>& scene)
    {
        auto found = std::find(mScenes.begin(), mScenes.end(), scene);
        if(found != mScenes.end()){
            mScenes.erase(found);
        }else{
            MS_LOG_ERROR("dont have a scene by the name: " + scene->getName());
        }
    }
    
    void SceneManager::clear()
    {
        mScenes.clear();
        mNextScene = nullptr;
        mCurrentScene = nullptr;
        resetFrameTime();
    }
    
}//end namespace mediasystem
