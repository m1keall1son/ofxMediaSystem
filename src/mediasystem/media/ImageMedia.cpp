//
//  ImageMedia.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "ImageMedia.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    ImageMedia::ImageMedia(Entity& context, const std::filesystem::path& path) :
        MediaBase(context),
        mFilename(path)
    {
        
    }
    
    ImageMedia::ImageMedia(Entity& context, int width, int height, ofImageType type) :
        MediaBase(context),
        mWidth(width),
        mHeight(height),
        mType(type)
    {
    }
    
    void ImageMedia::init()
    {
        mImage.clear();
        if(!mFilename.empty())
            mImage.load(mFilename);
        else{
            mImage.allocate(mWidth, mHeight, mType);
        }
        mWidth = mImage.getWidth();
        mHeight = mImage.getHeight();
        mIsInit = true;
    }
    
    glm::vec2 ImageMedia::getMediaSize() const
    {
        return glm::vec2(mWidth, mHeight);
    }
    
    void ImageMedia::bind()
    {
        //lazy init
        if(!mIsInit)
            init();
        
        mImage.bind();
    }
    
    void ImageMedia::unbind()
    {
        mImage.unbind();
    }
    
    bool ImageMedia::isInit()const
    {
        return mIsInit;
    }
    
    void ImageMedia::debugDraw(const ofRectangle& area, float fontsize)
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
        auto viewName = "IMAGE MEDIA";
        
        auto sw = font->stringWidth(viewName);
        auto line = 0;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 0);
        font->drawStringAsShapes(viewName, padding, line + padding + font->getAscenderHeight() );
        
        //path
        ofFill();
        ofSetColor(0, 0, 0);
        auto curPath = "path: "+ mFilename.string();
        
        sw = font->stringWidth(curPath);
        line = extra + (padding * 2);
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
        
        //size
        ofFill();
        ofSetColor(0, 0, 0);
        auto node = mNode.lock();
        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(mImage.getWidth())+" h: "+ofToString(mImage.getHeight())+"]";
        
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
    
    void ImageMedia::load(const std::filesystem::path& path)
    {
        mFilename = path;
        mIsInit = false;
    }

    
}//end namespace mediasystem
