//
//  CoreEvents.cpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#include "SceneEvents.h"
#include "mediasystem/core/Scene.h"

namespace mediasystem {
    
    SceneChange::SceneChange(Scene& current_scene, std::string next_scene):
        SceneEvent<SceneChange>(current_scene),
        mNextSceneName(std::move(next_scene))
    {}
    
    SceneChange::SceneChange(Scene& current_scene, std::shared_ptr<Scene> next_scene):
        SceneEvent<SceneChange>(current_scene),
        mNextScene(std::move(next_scene))
    {
        mNextSceneName = mNextScene->getName();
    }
    
    Update::Update(Scene& scene, uint64_t elapsedFrames, double elapsedTime, double lastFrameTime ):
        SceneEvent<Update>(scene),
        mElapsedFrames(elapsedFrames),
        mElapsedTime(elapsedTime),
        mLastFrameTime(lastFrameTime)
    {}
    
}//end namespace mediasystem
