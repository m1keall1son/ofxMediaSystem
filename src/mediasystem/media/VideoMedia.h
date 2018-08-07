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
    
    class VideoMedia : public IPlayableMedia {
    public:
        
        VideoMedia(std::filesystem::path path);
        
        void load()override;
        bool isLoaded()const override;
        void update(size_t frame, float elapedTime, float lastFrameTime)override;
        
        void play() override;
        void stop() override;
        void pause() override;
        void unpause()override;
        bool isPlaying()const override;
        bool isPaused()const override;
        
        void setLoop(bool loop) override;
        bool isLooping()const override;
        
        void bind()override;
        void unbind()override;
        
        glm::vec2 getMediaSize()const override;
        
    protected:
    
        ofVideoPlayer mVideoPlayer;
        std::filesystem::path mVideoPath;
        
    };
    
}// end namespace mediasystem
