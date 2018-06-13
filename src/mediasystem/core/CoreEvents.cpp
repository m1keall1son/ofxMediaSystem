//
//  CoreEvents.cpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#include "CoreEvents.h"
#include "mediasystem/core/Scene.h"

namespace mediasystem {
    
    SceneChange::SceneChange(std::string& scene):
        mNextSceneName(scene)
    {}
    
    SceneChange::SceneChange(std::shared_ptr<Scene> scene):
        mNextScene(std::move(scene))
    {
        mNextSceneName = mNextScene->getName();
    }
    
    UpdateEvent::UpdateEvent(uint64_t elapsedFrames, double elapsedTime, double lastFrameTime):
        mElapsedFrames(elapsedFrames),
        mElapsedTime(elapsedTime),
        mLastFrameTime(lastFrameTime)
    {}
    
}//end namespace mediasystem
