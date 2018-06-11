//
//  VideoView.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#include "VideoView.h"

namespace mediasystem {
    
    VideoView::VideoView(SceneBase* scene, const std::filesystem::path& path) :
        ViewBase(scene),
        mVideoPath(path)
    {
        
    }
    
    void VideoView::init()
    {
        if(!mVideoPath.empty()){
            mVideoPlayer.load(mVideoPath.string());
        }
    }
    
    void VideoView::draw()
    {
        mVideoPlayer.draw(0,0);
    }
    
    void VideoView::debugDraw()
    {
        //TODO
    }
    
    void VideoView::start()
    {
        mVideoPlayer.play();
    }
    
    void VideoView::stop()
    {
        mVideoPlayer.stop();
    }
    
    void VideoView::update()
    {
        mVideoPlayer.update();
    }
    
}//end namespace mediasystem
