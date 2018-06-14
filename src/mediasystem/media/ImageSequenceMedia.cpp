#include "ImageSequenceMedia.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/core/Scene.h"

namespace mediasystem {

    ImageSequenceMedia::ImageSequenceMedia(Entity& scene, float framerate, const Playable::Options& options) :
        ImageSequenceMediaBase(scene, framerate, options)
    {}

    ImageSequenceMedia::ImageSequenceMedia(Entity& scene, const std::filesystem::path & source,  float framerate, const Playable::Options& options) :
        ImageSequenceMediaBase(scene, framerate, options)
    {
        initPaths(source);
        setFramerate(framerate);
    }

    void ImageSequenceMedia::init()
    {
        mImages.clear();
        for ( auto & path : mSeqPaths ) {
            ofImage surface;
            surface.load(path);
            mImages.push_back(surface);
        }
        reset();
        mIsInit = true;
    }
    
    glm::vec2 ImageSequenceMedia::getMediaSize()const
    {
        if(!mImages.empty()){
            return glm::vec2(mImages.front().getWidth(), mImages.front().getHeight());
        }
        return glm::vec2(0);
    }
    
    bool ImageSequenceMedia::isInit()const
    {
        return mIsInit;
    }
    
    void ImageSequenceMedia::bind()
    {
        if(mImages.empty())return;
        auto index = getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1;
        mImages[index].bind();
    }
    
    void ImageSequenceMedia::unbind()
    {
        auto index = getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1;
        mImages[index].unbind();
    }
    
    void ImageSequenceMedia::debugDraw(const ofRectangle& area, float fontsize)
    {
        
        auto style = ofGetStyle();
        auto col = style.color;
        auto fill = style.bFill;
        
        ofColor playingColor;
        if(isPlaying()){
            playingColor = ofColor(255,0,255);
        }else if(isFinished()){
            playingColor = ofColor(0,127,255);
        }else{
            playingColor = ofColor(0,255,255);
        }
        
        ofSetColor(playingColor);
        ofNoFill();
        ofDrawRectangle(area);
        ofDrawLine(0, 0, area.width, area.height);
        ofDrawLine(area.width, 0, 0,area.height);
        auto font = getDebugFont(fontsize);
        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
        auto padding = 2;
        
        //type
        ofFill();
        ofSetColor(50, 50, 50);
        auto viewType = "IMAGE SEQUENCE MEDIA";
        
        auto sw = font->stringWidth(viewType);
        auto line = 0;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 0);
        font->drawStringAsShapes(viewType, padding, line + padding + font->getAscenderHeight() );
        
        //current file
        ofFill();
        ofSetColor(0, 0, 0);
        auto curPath = "current path: " + mSeqPaths[getState().currentFrame].string();
        
        sw = font->stringWidth(curPath);
        line = extra + (padding * 2);

        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
        
        //current file
        ofFill();
        ofSetColor(0, 0, 0);
        std::string status = "status: ";
        
        if(isPaused()){
            status += " PAUSED";
        }else if(isPlaying()){
            status += " PLAYING";
        }else if(isFinished()){
            status += " FINISHED";
        }else{
            status += " STOPPED";
        }
        
        sw = font->stringWidth(status);
        line = (extra + (padding * 2))*2;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(playingColor);
        font->drawStringAsShapes(status, padding, line + padding + font->getAscenderHeight() );
        
        //size
        ofFill();
        ofSetColor(0, 0, 0);
        auto node = mNode.lock();
        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(getCurrentTexture()->getWidth())+" h: "+ofToString(getCurrentTexture()->getHeight())+"]";
        
        sw = font->stringWidth(nodesize);
        line = (extra + (padding * 2))*3;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
        
        //completeness
        ofFill();
        ofSetColor(0, 0, 0);
        auto position = "complete: " + ofToString(getState().getPlaybackPosition()*100) + "% frame: " + ofToString(getState().currentFrame);
        
        sw = font->stringWidth(position);
        line = (extra + (padding * 2))*4;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(position, padding, line + padding + font->getAscenderHeight() );
        
        ofSetColor(127, 255, 255);
        ofDrawRectangle(0, area.height-10, area.width*getState().getPlaybackPosition(), 10);
        
        //pop
        ofSetColor(col);
        if(fill)
            ofFill();
        else
            ofNoFill();
    }

    ofTexture* ImageSequenceMedia::getCurrentTexture()
    {
        return !mImages.empty() ? &mImages[getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1].getTexture() : nullptr;
    }
    
}//namespace mediasystem
