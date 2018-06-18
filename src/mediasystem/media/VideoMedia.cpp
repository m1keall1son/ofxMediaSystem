//
//  VideoView.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/9/18.
//

#include "VideoMedia.h"
#include "mediasystem/core/Scene.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    VideoMedia::VideoMedia(Entity& context, std::filesystem::path path) :
        MediaBase(context),
        mVideoPath(std::move(path))
    {
        auto& scene = context.getScene();
        scene.addDelegate<Start>(EventDelegate::create<VideoMedia, &VideoMedia::start>(this));
        scene.addDelegate<Stop>(EventDelegate::create<VideoMedia, &VideoMedia::stop>(this));
        scene.addDelegate<Update>(EventDelegate::create<VideoMedia, &VideoMedia::update>(this));
    }
    
    VideoMedia::~VideoMedia()
    {
        auto& scene = mContext.getScene();
        scene.removeDelegate<Start>(EventDelegate::create<VideoMedia, &VideoMedia::start>(this));
        scene.removeDelegate<Stop>(EventDelegate::create<VideoMedia, &VideoMedia::stop>(this));
        scene.removeDelegate<Update>(EventDelegate::create<VideoMedia, &VideoMedia::update>(this));
    }
    
    glm::vec2 VideoMedia::getMediaSize() const
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
    
    void VideoMedia::debugDraw(const ofRectangle& area, float fontsize)
    {
        //push
        auto style = ofGetStyle();
        auto col = style.color;
        auto fill = style.bFill;
        //draw
        ofSetColor(255, 255, 0, 255);
        ofNoFill();
        ofDrawRectangle(area);
        ofDrawLine(0, 0, area.width, area.height);
        ofDrawLine(area.width, 0, 0, area.height);
        auto font = getDebugFont(fontsize);
        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
        auto padding = 2;
        
        //current type
        ofFill();
        ofSetColor(50, 50, 50);
        auto viewName = "VIDEO MEDIA";
        
        auto sw = font->stringWidth(viewName);
        auto line = 0;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 0);
        font->drawStringAsShapes(viewName, padding, line + padding + font->getAscenderHeight() );
        
        //path
        ofFill();
        ofSetColor(0, 0, 0);
        auto curPath = "path: "+ mVideoPath.string();
        
        sw = font->stringWidth(curPath);
        line = extra + (padding * 2);
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
        
        //size
        ofFill();
        ofSetColor(0, 0, 0);
        auto node = mNode.lock();
        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(mVideoPlayer.getWidth())+" h: "+ofToString(mVideoPlayer.getHeight())+"]";
        
        sw = font->stringWidth(nodesize);
        line = (extra + (padding * 2))*2;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
        
        //pop
        ofSetColor(col);
        if(fill)
            ofFill();
        else
            ofNoFill();
    }
    
    EventStatus VideoMedia::start(const IEventRef&)
    {
        if(!mIsInit)
            init();
        mVideoPlayer.play();
        return EventStatus::SUCCESS;
    }
    
    EventStatus VideoMedia::stop(const IEventRef&)
    {
        mVideoPlayer.stop();
        return EventStatus::SUCCESS;
    }
    
    EventStatus VideoMedia::update(const IEventRef&)
    {
        if(!mIsInit)
            init();
        mVideoPlayer.update();
        return EventStatus::SUCCESS;
    }
    
}//end namespace mediasystem
