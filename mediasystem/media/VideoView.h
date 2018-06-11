//
//  VideoView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#pragma once
#include "mediasystem/behaviors/ViewBase.h"
#include "ofMain.h"

namespace mediasystem {
    
    class VideoView : public ViewBase {
    public:
        
        VideoView(SceneBase* scene, const std::filesystem::path& path);
        
        void init()override;
        void draw()override;
        void debugDraw()override;
        
        void start()override;
        void stop()override;
        void update()override;
        
    protected:
        
        ofVideoPlayer mVideoPlayer;
        std::filesystem::path mVideoPath;
        
    };
    
}// end namespace mediasystem
