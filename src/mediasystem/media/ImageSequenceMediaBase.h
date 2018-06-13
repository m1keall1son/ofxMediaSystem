//
//  ImageSequenceViewBase.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#pragma once

#include "ImageSequenceBase.h"
#include "mediasystem/media/MediaBase.h"

namespace mediasystem {
    
    class Scene;
    
    class ImageSequenceMediaBase : public MediaBase, public ImageSequenceBase {
    public:
      
        ImageSequenceMediaBase(Entity& context, float fps, const Playable::Options& options = Playable::Options() );
        virtual ~ImageSequenceMediaBase();
        
    protected:
                
        virtual void start(Scene*);
        virtual void stop(Scene*);
        virtual void update(Scene*);
        
        bool mIsInit{false};
        
    };
    
}//end namespace mediasystem
