//
//  MediaDebug.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 8/7/18.
//

#include "MediaDebug.h"

namespace mediasystem {
    
    
    //    void VideoMedia::debugDraw(const ofRectangle& area, float fontsize)
    //    {
    //        //push
    //        auto style = ofGetStyle();
    //        auto col = style.color;
    //        auto fill = style.bFill;
    //        //draw
    //        ofSetColor(255, 255, 0, 255);
    //        ofNoFill();
    //        ofDrawRectangle(area);
    //        ofDrawLine(0, 0, area.width, area.height);
    //        ofDrawLine(area.width, 0, 0, area.height);
    //        auto font = getDebugFont(fontsize);
    //        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
    //        auto padding = 2;
    //
    //        //current type
    //        ofFill();
    //        ofSetColor(50, 50, 50);
    //        auto viewName = "VIDEO MEDIA";
    //
    //        auto sw = font->stringWidth(viewName);
    //        auto line = 0;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 0);
    //        font->drawStringAsShapes(viewName, padding, line + padding + font->getAscenderHeight() );
    //
    //        //path
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto curPath = "path: "+ mVideoPath.string();
    //
    //        sw = font->stringWidth(curPath);
    //        line = extra + (padding * 2);
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
    //
    //        //size
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto node = mNode.lock();
    //        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(mVideoPlayer.getWidth())+" h: "+ofToString(mVideoPlayer.getHeight())+"]";
    //
    //        sw = font->stringWidth(nodesize);
    //        line = (extra + (padding * 2))*2;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
    //
    //        //pop
    //        ofSetColor(col);
    //        if(fill)
    //            ofFill();
    //        else
    //            ofNoFill();
    //    }
    
    
    
    //    void ImageMedia::debugDraw(const ofRectangle& area, float fontsize)
    //    {
    //        //push
    //        auto style = ofGetStyle();
    //        auto col = style.color;
    //        auto fill = style.bFill;
    //        //draw
    //        ofSetColor(255, 255, 0, 255);
    //        ofNoFill();
    //        ofDrawRectangle(area);
    //        ofDrawLine(0, 0, area.width, area.height);
    //        ofDrawLine(area.width, 0, 0, area.height);
    //        auto font = getDebugFont(fontsize);
    //        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
    //        auto padding = 2;
    //
    //        //current type
    //        ofFill();
    //        ofSetColor(50, 50, 50);
    //        auto viewName = "IMAGE MEDIA";
    //
    //        auto sw = font->stringWidth(viewName);
    //        auto line = 0;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 0);
    //        font->drawStringAsShapes(viewName, padding, line + padding + font->getAscenderHeight() );
    //
    //        //path
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto curPath = "path: "+ mFilename.string();
    //
    //        sw = font->stringWidth(curPath);
    //        line = extra + (padding * 2);
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
    //
    //        //size
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto node = mNode.lock();
    //        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(mImage.getWidth())+" h: "+ofToString(mImage.getHeight())+"]";
    //
    //        sw = font->stringWidth(nodesize);
    //        line = (extra + (padding * 2))*2;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
    //
    //        //pop
    //        ofSetColor(col);
    //        if(fill)
    //            ofFill();
    //        else
    //            ofNoFill();
    //    }
    
    //    void StreamingImageSequenceMedia::debugDraw(const ofRectangle& area, float fontsize)
    //    {
    //        if(!mCurrentImage.isAllocated()) return;
    //        //push
    //        auto style = ofGetStyle();
    //        auto col = style.color;
    //        auto fill = style.bFill;
    //
    //        ofColor playingColor;
    //        if(isPlaying()){
    //            playingColor = ofColor(255,0,255);
    //        }else if(isFinished()){
    //            playingColor = ofColor(0,127,255);
    //        }else{
    //            playingColor = ofColor(0,255,255);
    //        }
    //        //draw
    //        ofSetColor(playingColor);
    //        ofNoFill();
    //        ofDrawRectangle(area);
    //        ofDrawLine(0, 0, area.width, area.height);
    //        ofDrawLine(area.width, 0, 0, area.height);
    //        auto font = getDebugFont(fontsize);
    //        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
    //        auto padding = 2;
    //
    //        //current type
    //        ofFill();
    //        ofSetColor(50, 50, 50);
    //        auto viewName = "STREAMING IMAGE SEQUENCE MEDIA";
    //
    //        auto sw = font->stringWidth(viewName);
    //        auto line = 0;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 0);
    //        font->drawStringAsShapes(viewName, padding, line + padding + font->getAscenderHeight() );
    //
    //        //current file
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto curPath = "current path: " + mSeqPaths[getState().currentFrame].string();
    //
    //        sw = font->stringWidth(curPath);
    //        line = extra + (padding * 2);
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
    //
    //        //current file
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        std::string status = "status: ";
    //
    //        if(isPaused()){
    //            status += " PAUSED";
    //        }else if(isPlaying()){
    //            status += " PLAYING";
    //        }else if(isFinished()){
    //            status += " FINISHED";
    //        }else{
    //            status += " STOPPED";
    //        }
    //
    //        sw = font->stringWidth(status);
    //        line = (extra + (padding * 2))*2;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(playingColor);
    //        font->drawStringAsShapes(status, padding, line + padding + font->getAscenderHeight() );
    //
    //        //size
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto node = mNode.lock();
    //        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(mCurrentImage.getWidth())+" h: "+ofToString(mCurrentImage.getHeight())+"]";
    //
    //        sw = font->stringWidth(nodesize);
    //        line = (extra + (padding * 2))*3;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
    //
    //        //completeness
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto position = "complete: " + ofToString(getState().getPlaybackPosition()*100) + "% frame: " + ofToString(getState().currentFrame);
    //
    //        sw = font->stringWidth(position);
    //        line = (extra + (padding * 2))*4;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(position, padding, line + padding + font->getAscenderHeight() );
    //
    //        ofSetColor(127, 255, 255);
    //        ofDrawRectangle(0, area.height-10, area.width*getState().getPlaybackPosition(), 10);
    //
    //        //missed frames
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto missed = "missed frames: " + ofToString(mMissedFrames) + " queue size: " + ofToString(mQueue.size());
    //
    //        sw = font->stringWidth(missed);
    //        line = (extra + (padding * 2))*5;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255, 255);
    //        font->drawStringAsShapes(missed, padding, line + padding + font->getAscenderHeight() );
    //
    //        //pop
    //        ofSetColor(col);
    //        if(fill)
    //            ofFill();
    //        else
    //            ofNoFill();
    //    }
    
    //    void ImageSequenceMedia::debugDraw(const ofRectangle& area, float fontsize)
    //    {
    //
    //        auto style = ofGetStyle();
    //        auto col = style.color;
    //        auto fill = style.bFill;
    //
    //        ofColor playingColor;
    //        if(isPlaying()){
    //            playingColor = ofColor(255,0,255);
    //        }else if(isFinished()){
    //            playingColor = ofColor(0,127,255);
    //        }else{
    //            playingColor = ofColor(0,255,255);
    //        }
    //
    //        ofSetColor(playingColor);
    //        ofNoFill();
    //        ofDrawRectangle(area);
    //        ofDrawLine(0, 0, area.width, area.height);
    //        ofDrawLine(area.width, 0, 0,area.height);
    //        auto font = getDebugFont(fontsize);
    //        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
    //        auto padding = 2;
    //
    //        //type
    //        ofFill();
    //        ofSetColor(50, 50, 50);
    //        auto viewType = "IMAGE SEQUENCE MEDIA";
    //
    //        auto sw = font->stringWidth(viewType);
    //        auto line = 0;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 0);
    //        font->drawStringAsShapes(viewType, padding, line + padding + font->getAscenderHeight() );
    //
    //        //current file
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto curPath = "current path: " + mSeqPaths[getState().currentFrame].string();
    //
    //        sw = font->stringWidth(curPath);
    //        line = extra + (padding * 2);
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
    //
    //        //current file
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        std::string status = "status: ";
    //
    //        if(isPaused()){
    //            status += " PAUSED";
    //        }else if(isPlaying()){
    //            status += " PLAYING";
    //        }else if(isFinished()){
    //            status += " FINISHED";
    //        }else{
    //            status += " STOPPED";
    //        }
    //
    //        sw = font->stringWidth(status);
    //        line = (extra + (padding * 2))*2;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(playingColor);
    //        font->drawStringAsShapes(status, padding, line + padding + font->getAscenderHeight() );
    //
    //        //size
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto node = mNode.lock();
    //        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(getCurrentTexture()->getWidth())+" h: "+ofToString(getCurrentTexture()->getHeight())+"]";
    //
    //        sw = font->stringWidth(nodesize);
    //        line = (extra + (padding * 2))*3;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
    //
    //        //completeness
    //        ofFill();
    //        ofSetColor(0, 0, 0);
    //        auto position = "complete: " + ofToString(getState().getPlaybackPosition()*100) + "% frame: " + ofToString(getState().currentFrame);
    //
    //        sw = font->stringWidth(position);
    //        line = (extra + (padding * 2))*4;
    //
    //        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
    //        ofSetColor(255, 255, 255);
    //        font->drawStringAsShapes(position, padding, line + padding + font->getAscenderHeight() );
    //
    //        ofSetColor(127, 255, 255);
    //        ofDrawRectangle(0, area.height-10, area.width*getState().getPlaybackPosition(), 10);
    //
    //        //pop
    //        ofSetColor(col);
    //        if(fill)
    //            ofFill();
    //        else
    //            ofNoFill();
    //    }

    
}//end namespace mediasystem
