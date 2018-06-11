#include "ImageSequenceView.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/util/Log.h"

namespace mediasystem {

    ImageSequenceView::ImageSequenceView(SceneBase* scene, float framerate, const Playable::Options& options) :
        ImageSequenceViewBase(scene, framerate, options)
    {}

    ImageSequenceView::ImageSequenceView(SceneBase* scene, const std::filesystem::path & source,  float framerate, const Playable::Options& options) :
        ImageSequenceViewBase(scene, framerate, options)
    {
        initPaths(source);
        setFramerate(framerate);
    }

    void ImageSequenceView::init()
    {
        mImages.clear();
        for ( auto & path : mSeqPaths ) {
            ofImage surface;
            surface.load(path);
            mSize = glm::vec2(surface.getWidth(),surface.getHeight());
            mImages.push_back(surface);
        }
        reset();
    }
    
    void ImageSequenceView::draw()
    {
        if(mImages.empty())return;
        auto index = getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1;
        mImages[index].draw(0,0);
    }
    
    void ImageSequenceView::debugDraw()
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
        ofDrawRectangle(0, 0, getCurrentTexture()->getWidth(), getCurrentTexture()->getHeight());
        ofDrawLine(0, 0, getCurrentTexture()->getWidth(), getCurrentTexture()->getHeight());
        ofDrawLine(getCurrentTexture()->getWidth(), 0, 0, getCurrentTexture()->getHeight());
        auto font = getDebugFont();
        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
        auto padding = 2;
        
        //type
        ofFill();
        ofSetColor(50, 50, 50);
        auto viewType = "IMAGE SEQUENCE VIEW";
        
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
        auto area = "[x: "+ofToString(getPosition().x)+" y: "+ofToString(getPosition().y)+" w: "+ofToString(getCurrentTexture()->getWidth())+" h: "+ofToString(getCurrentTexture()->getHeight())+"]";
        
        sw = font->stringWidth(area);
        line = (extra + (padding * 2))*3;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(area, padding, line + padding + font->getAscenderHeight() );
        
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
        ofDrawRectangle(0, getCurrentTexture()->getHeight()-10, getCurrentTexture()->getHeight()*getState().getPlaybackPosition(), 10);
        
        //pop
        ofSetColor(col);
        if(fill)
            ofFill();
        else
            ofNoFill();
    }

    ofTexture* ImageSequenceView::getCurrentTexture()
    {
        return !mImages.empty() ? &mImages[getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1].getTexture() : nullptr;
    }
    
}//namespace mediasystem
