//
//  ImageSequenceViewBase.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#pragma once

#include "ImageSequenceBase.h"
#include "mediasystem/behaviors/ViewBase.h"

namespace mediasystem {
    
    class SceneBase;
    
    class ImageSequenceViewBase : public ViewBase, public ImageSequenceBase {
    public:
      
        ImageSequenceViewBase(SceneBase* scene, float fps, const Playable::Options& options = Playable::Options() );
        
        virtual void start()override;
        virtual void stop()override;
        virtual void update()override;
        
    };
    
}//end namespace mediasystem
