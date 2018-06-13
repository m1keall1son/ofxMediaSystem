//
//  VideoView.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#include "VideoMedia.h"
#include "mediasystem/core/Scene.h"

namespace mediasystem {
    
    VideoMedia::VideoMedia(Entity& context, std::filesystem::path path) :
        MediaBase(context),
        mVideoPath(std::move(path))
    {
        auto& scene = context.getScene();
        scene.addDelegate(Scene::Events::START, SceneEventDelegate::create<VideoMedia, &VideoMedia::start>(this));
        scene.addDelegate(Scene::Events::STOP, SceneEventDelegate::create<VideoMedia, &VideoMedia::stop>(this));
        scene.addDelegate(Scene::Events::UPDATE, SceneEventDelegate::create<VideoMedia, &VideoMedia::update>(this));
    }
    
    VideoMedia::~VideoMedia()
    {
        auto& scene = mContext.getScene();
        scene.removeDelegate(Scene::Events::START, SceneEventDelegate::create<VideoMedia, &VideoMedia::start>(this));
        scene.removeDelegate(Scene::Events::STOP, SceneEventDelegate::create<VideoMedia, &VideoMedia::stop>(this));
        scene.removeDelegate(Scene::Events::UPDATE, SceneEventDelegate::create<VideoMedia, &VideoMedia::update>(this));
    }
    
    glm::vec2 VideoMedia::getMediaSize()
    {
        return glm::vec2(mVideoPlayer.getWidth(), mVideoPlayer.getHeight());
    }
    
    void VideoMedia::init()
    {
        if(!mVideoPath.empty()){
            mVideoPlayer.load(mVideoPath.string());
        }
        mIsInit = true;
    }
    
    void VideoMedia::bind()
    {
        mVideoPlayer.bind();
    }
    
    void VideoMedia::unbind()
    {
        mVideoPlayer.unbind();
    }
    
    bool VideoMedia::isInit() const
    {
        return mIsInit;
    }
    
    void VideoMedia::debugDraw()
    {
        //TODO
    }
    
    void VideoMedia::start(Scene*)
    {
        if(!mIsInit)
            init();
        mVideoPlayer.play();
    }
    
    void VideoMedia::stop(Scene*)
    {
        mVideoPlayer.stop();
    }
    
    void VideoMedia::update(Scene*)
    {
        if(!mIsInit)
            init();
        mVideoPlayer.update();
    }
    
}//end namespace mediasystem
