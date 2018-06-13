//
//  ImageSequenceViewBase.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#include "ImageSequenceMediaBase.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/core/Scene.h"
#include "mediasystem/core/CoreEvents.h"

namespace mediasystem {
    
    ImageSequenceMediaBase::ImageSequenceMediaBase(Entity& context, float fps, const Playable::Options& options):
        MediaBase(context),
        ImageSequenceBase(fps, options)
    {
        auto& scene = context.getScene();
        scene.addDelegate(Scene::Events::START, SceneEventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::start>(this));
        scene.addDelegate(Scene::Events::STOP, SceneEventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::stop>(this));
        scene.addDelegate(Scene::Events::UPDATE, SceneEventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::update>(this));
    }
    
    ImageSequenceMediaBase::~ImageSequenceMediaBase()
    {
        auto& scene = mContext.getScene();
        scene.removeDelegate(Scene::Events::START, SceneEventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::start>(this));
        scene.removeDelegate(Scene::Events::STOP, SceneEventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::stop>(this));
        scene.removeDelegate(Scene::Events::UPDATE, SceneEventDelegate::create<ImageSequenceMediaBase, &ImageSequenceMediaBase::update>(this));
    }
    
    void ImageSequenceMediaBase::start(Scene*)
    {
        if(!mIsInit){
            init();
            mIsInit = true;
        }
        play();
    }
    
    void ImageSequenceMediaBase::stop(Scene*)
    {
        ImageSequenceBase::stop();
    }
    
    void ImageSequenceMediaBase::update(Scene*)
    {
        if(!mIsInit){
            init();
            mIsInit = true;
        }
        auto val = ofGetLastFrameTime();
        step(val);
    }
    
}//end namespace mediasystem
