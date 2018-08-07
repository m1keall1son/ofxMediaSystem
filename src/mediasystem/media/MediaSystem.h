//
//  MediaSystem.h
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#pragma once
#include "mediasystem/core/Scene.h"
#include "mediasystem/util/Manager.hpp"
#include "mediasystem/media/IMedia.h"

namespace mediasystem {
    
    class MediaSystem : public Manager<IMedia> {
    public:
        
        MediaSystem(Scene& scene);
        ~MediaSystem();
        
        void loadMedia();
        
    private:
        Scene& mScene;
        EventStatus onUpdate(const IEventRef& event);
    };
    
    
}//end namespace mediasystem
