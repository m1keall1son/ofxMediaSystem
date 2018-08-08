//
//  MediaSystem.h
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#pragma once
#include "mediasystem/core/Scene.h"
#include "mediasystem/media/imgseq/ImageSequence.h"

namespace mediasystem {
    
    class MediaUpdateSystem {
    public:
        
        MediaUpdateSystem(Scene& scene);
        ~MediaUpdateSystem();
        
    private:
        
        Scene& mScene;
        EventStatus onUpdate(const IEventRef& event);

        ComponentMap<ofVideoPlayer> mVideoPlayers;
        ComponentMap<ImageSequence> mImageSequences;
        
    };
    
    
}//end namespace mediasystem
