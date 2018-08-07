//
//  VideoView.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#include "VideoMedia.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    VideoMedia::VideoMedia(std::filesystem::path path) :
        mVideoPath(std::move(path))
    {
    }
    
    glm::vec2 VideoMedia::getMediaSize() const
    {
        return glm::vec2(getWidth(), getHeight());
    }
    
    void VideoMedia::load()
    {
        if(!mVideoPath.empty()){
            ofVideoPlayer::load(mVideoPath.string());
        }
    }
    
    void VideoMedia::bind()
    {
        ofVideoPlayer::bind();
    }
    
    void VideoMedia::unbind()
    {
        ofVideoPlayer::unbind();
    }
    
    bool VideoMedia::isLoaded() const
    {
        return ofVideoPlayer::isLoaded();
    }
    
}//end namespace mediasystem
