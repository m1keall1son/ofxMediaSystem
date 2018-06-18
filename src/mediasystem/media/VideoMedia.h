//
//  VideoView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#pragma once
#include "mediasystem/media/MediaBase.h"
#include "ofMain.h"

namespace mediasystem {
    
    class Scene;
    
    class VideoMedia : public MediaBase {
    public:
        
        VideoMedia(Entity& entity, std::filesystem::path path);
        ~VideoMedia();
        
        void init()override;
        void bind()override;
        void unbind()override;
        bool isInit()const override;
        void debugDraw(const ofRectangle& area, float fontsize)override;
        
        glm::vec2 getMediaSize()const override;
        
    protected:
        
        EventStatus start(const IEventRef&);
        EventStatus stop(const IEventRef&);
        EventStatus update(const IEventRef&);
        
        ofVideoPlayer mVideoPlayer;
        std::filesystem::path mVideoPath;
        bool mIsInit{false};
        
    };
    
}// end namespace mediasystem
