//
//  MediaView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/4/18.
//

#pragma once

#include "ViewBase.h"

namespace mediasystem {
    
    class MediaView : public ViewBase {
    public:
        
        virtual ~MediaView() = default;
        
        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;

    private:
        
    };
    
}//end namespace mediasystem
