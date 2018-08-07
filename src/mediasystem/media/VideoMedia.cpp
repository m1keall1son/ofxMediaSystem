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
        return glm::vec2(mVideoPlayer.getWidth(), mVideoPlayer.getHeight());
    }
    
    void VideoMedia::load()
    {
        if(!mVideoPath.empty()){
            mVideoPlayer.load(mVideoPath.string());
        }
    }
    
    void VideoMedia::bind()
    {
        mVideoPlayer.bind();
    }
    
    void VideoMedia::unbind()
    {
        mVideoPlayer.unbind();
    }
    
    bool VideoMedia::isLoaded() const
    {
        return mVideoPlayer.isLoaded();
    }
 
    void VideoMedia::play()
    {
        mVideoPlayer.play();
    }
    
    void VideoMedia::stop()
    {
        mVideoPlayer.stop();
    }
    
    void VideoMedia::pause()
    {
        mVideoPlayer.setPaused(true);
    }
    
    void VideoMedia::unpause()
    {
        mVideoPlayer.setPaused(false);
    }
    
    bool VideoMedia::isPlaying()const
    {
        return mVideoPlayer.isPlaying();
    }
    
    bool VideoMedia::isPaused()const
    {
        return mVideoPlayer.isPaused();
    }
    
    void VideoMedia::setLoop(bool loop)
    {
        mVideoPlayer.setLoopState(ofLoopType::OF_LOOP_NORMAL);
    }
    
    bool VideoMedia::isLooping()const
    {
        return mVideoPlayer.getLoopState() != ofLoopType::OF_LOOP_NONE;
    }

    void VideoMedia::update(size_t frame, float elapedTime, float lastFrameTime)
    {
        mVideoPlayer.update();
    }
    
}//end namespace mediasystem
