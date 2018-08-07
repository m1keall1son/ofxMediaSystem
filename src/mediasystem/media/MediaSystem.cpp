//
//  MediaSystem.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#include "MediaSystem.h"

namespace mediasystem {
    
    MediaSystem::MediaSystem(Scene& scene):
       mScene(scene)
    {
        mScene.addDelegate<Update>(EventDelegate::create<MediaSystem, &MediaSystem::onUpdate>(this));
    }
    
    MediaSystem::~MediaSystem()
    {
        mScene.removeDelegate<Update>(EventDelegate::create<MediaSystem, &MediaSystem::onUpdate>(this));
    }
    
    void MediaSystem::loadMedia()
    {
        for(auto & media : getManagedItems()){
            if(!media.second->isLoaded())
                media.second->load();
        }
    }
    
    EventStatus MediaSystem::onUpdate(const IEventRef& event)
    {
        auto update = std::static_pointer_cast<Update>(event);
        for(auto & media : getManagedItems()){
            if(!media.second->isLoaded())
                media.second->load();
            if(media.second->isPlayable()){
                media.second->update(update->getElapsedFrames(), update->getElapsedTime(), update->getLastFrameTime());
            }
        }
        return EventStatus::SUCCESS;
    }
    
}//end namespace mediasystem
