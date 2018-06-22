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
        scene.addDelegate<Start>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::start>(this));
        scene.addDelegate<Stop>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::stop>(this));
        scene.addDelegate<Update>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::update>(this));
    }
    
    ImageSequenceMediaBase::~ImageSequenceMediaBase()
    {
        auto& scene = mContext.getScene();
        scene.removeDelegate<Start>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::start>(this));
        scene.removeDelegate<Stop>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::stop>(this));
        scene.removeDelegate<Update>(EventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::update>(this));
    }
    
    EventStatus ImageSequenceMediaBase::start(const IEventRef&)
    {
        if(!mIsInit){
            init();
            mIsInit = true;
        }
        play();
        return EventStatus::SUCCESS;
    }
    
    EventStatus ImageSequenceMediaBase::stop(const IEventRef&)
    {
        ImageSequenceBase::stop();
        return EventStatus::SUCCESS;
    }
    
    EventStatus ImageSequenceMediaBase::update(const IEventRef& event)
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
