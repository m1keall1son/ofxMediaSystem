//
//  VideoView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#pragma once
#include "mediasystem/media/IMedia.h"
#include "ofMain.h"

namespace mediasystem {
    
    class Scene;
    
    class VideoMedia : public IMedia, public ofVideoPlayer {
    public:
        
        VideoMedia(std::filesystem::path path);
        
        void load()override;
        bool isLoaded()const override;
        
        void bind()override;
        void unbind()override;
        
        void update(size_t frame, float elapsedTime, float lastFrameTime)override;
        
        glm::vec2 getMediaSize()const override;
        
    protected:
    
        std::filesystem::path mVideoPath;
        
    };
    
}// end namespace mediasystem
