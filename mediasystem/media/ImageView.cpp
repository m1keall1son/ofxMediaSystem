//
//  ImageView.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "ImageView.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    ImageView::ImageView(SceneBase* scene) : ViewBase(scene)
    {
        
    }
    
    ImageView::ImageView(SceneBase* scene, const std::filesystem::path& path) :
        ViewBase(scene),
        mFilename(path)
    {
        
    }
    
    ImageView::ImageView(SceneBase* scene, int width, int height, ofImageType type) :
        ViewBase(scene),
        mWidth(width),
        mHeight(height),
        mType(type)
    {
    }
    
    ImageView::~ImageView()
    {
        
    }
    
    void ImageView::init()
    {
        mImage.clear();
        if(!mFilename.empty())
            mImage.load(mFilename);
        else{
            mImage.allocate(mWidth, mHeight, mType);
        }
        mWidth = mImage.getWidth();
        mHeight = mImage.getHeight();
    }

    void ImageView::draw()
    {
        mImage.draw(0, 0, mWidth, mHeight);
    }
    
    void ImageView::debugDraw()
    {
        //push
        auto style = ofGetStyle();
        auto col = style.color;
        auto fill = style.bFill;
        //draw
        ofSetColor(255, 255, 0, 255);
        ofNoFill();
        ofDrawRectangle(0, 0, mWidth, mHeight);
        ofDrawLine(0, 0, mWidth, mHeight);
        ofDrawLine(mWidth, 0, 0, mHeight);
        auto font = getDebugFont();
        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
        auto padding = 2;
        
        //current type
        ofFill();
        ofSetColor(50, 50, 50);
        auto viewName = "IMAGE VIEW";
        
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
        auto area = "[x: "+ofToString(getPosition().x)+" y: "+ofToString(getPosition().y)+" w: "+ofToString(mWidth)+" h: "+ofToString(mHeight)+"]";
        
        sw = font->stringWidth(area);
        line = (extra + (padding * 2))*2;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(area, padding, line + padding + font->getAscenderHeight() );
        
        //pop
        ofSetColor(col);
        if(fill)
            ofFill();
        else
            ofNoFill();
    }
    
    void ImageView::load(const std::filesystem::path& path)
    {
        mFilename = path;
        if(isInit()){
            init();
        }
    }

    
}//end namespace mediasystem
