//
//  ImageSequenceViewBase.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#include "ImageSequenceMediaBase.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/core/Scene.h"
#include "mediasystem/events/SceneEvents.h"

namespace mediasystem {
    
    ImageSequenceMediaBase::ImageSequenceMediaBase(Entity& context, float fps, const Playable::Options& options):
        MediaBase(context),
        ImageSequenceBase(fps, options)
    {
        auto& scene = context.getScene();
        scene.addDelegate<Start>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::startPlayback>(this));
        scene.addDelegate<Stop>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::stopPlayback>(this));
        scene.addDelegate<Update>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::updateMedia>(this));
    }
    
    ImageSequenceMediaBase::~ImageSequenceMediaBase()
    {
        auto& scene = mContext.getScene();
        scene.removeDelegate<Start>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::startPlayback>(this));
        scene.removeDelegate<Stop>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::stopPlayback>(this));
        scene.removeDelegate<Update>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::updateMedia>(this));
    }
    
    EventStatus ImageSequenceMediaBase::startPlayback(const IEventRef&)
    {
        if(!mIsInit){
            init();
            mIsInit = true;
        }
        play();
        return EventStatus::SUCCESS;
    }
    
    EventStatus ImageSequenceMediaBase::stopPlayback(const IEventRef&)
    {
        ImageSequenceBase::stop();
        return EventStatus::SUCCESS;
    }
    
    EventStatus ImageSequenceMediaBase::updateMedia(const IEventRef& event)
    {
        auto updateEvent = std::static_pointer_cast<Update>(event);
        if(!mIsInit){
            init();
            mIsInit = true;
        }
        auto val = updateEvent->getLastFrameTime();
        step(val);
        return EventStatus::SUCCESS;
    }
    
}//end namespace mediasystem
