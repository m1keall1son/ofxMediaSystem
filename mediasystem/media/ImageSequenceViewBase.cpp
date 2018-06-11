//
//  ImageSequenceViewBase.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#include "ImageSequenceViewBase.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    ImageSequenceViewBase::ImageSequenceViewBase(SceneBase* scene, float fps, const Playable::Options& options):
        ViewBase(scene),
        ImageSequenceBase(fps, options)
    {}
    
    void ImageSequenceViewBase::start()
    {
        play();
    }
    
    void ImageSequenceViewBase::stop()
    {
        ImageSequenceBase::stop();
    }
    
    void ImageSequenceViewBase::update()
    {
        auto val = getLastFrameTime();
        step(val);
    }
    
}//end namespace mediasystem
