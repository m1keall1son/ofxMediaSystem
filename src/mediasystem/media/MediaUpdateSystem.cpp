//
//  MediaSystem.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#include "MediaUpdateSystem.h"

namespace mediasystem {
    
    MediaUpdateSystem::MediaUpdateSystem(Scene& scene):
       mScene(scene)
    {
        mVideoPlayers = mScene.getComponents<ofVideoPlayer>();
        mImageSequences = mScene.getComponents<ImageSequence>();
        mScene.addDelegate<Update>(EventDelegate::create<MediaUpdateSystem, &MediaUpdateSystem::onUpdate>(this));
    }
    
    MediaUpdateSystem::~MediaUpdateSystem()
    {
        mScene.removeDelegate<Update>(EventDelegate::create<MediaUpdateSystem, &MediaUpdateSystem::onUpdate>(this));
    }
    
    EventStatus MediaUpdateSystem::onUpdate(const IEventRef& event)
    {
        auto update = std::static_pointer_cast<Update>(event);
        
        auto it = mVideoPlayers.iter();
        while (auto video = it.next()) {
            video->update();
        }
        
        auto seqit = mImageSequences.iter();
        while (auto seq = seqit.next()) {
            seq->step(update->getLastFrameTime());
        }
        
        return EventStatus::SUCCESS;
    }
    
}//end namespace mediasystem
