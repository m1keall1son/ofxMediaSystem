//
//  ScreenBounds.hpp
//  WallTest
//
//  Created by Michael Allison on 6/25/18.
//

#pragma once

#include "ofMain.h"
#include "mediasystem/core/Entity.h"

namespace mediasystem {
    
    class ScreenBounds {
    public:
        
        ScreenBounds(Entity& context, ofRectangle rect):
            mContext(context),
            mCachedBounds(rect),
            mSize(rect.width, rect.height),
            mOrigin(rect.x, rect.y)
        {
            mContext.getScene().addDelegate<Update>(EventDelegate::create<ScreenBounds,&ScreenBounds::onUpdate>(this));
        }
        
        ~ScreenBounds()
        {
            mContext.getScene().removeDelegate<Update>(EventDelegate::create<ScreenBounds,&ScreenBounds::onUpdate>(this));
        }
        
        void update(){
            if(auto node = mContext.getComponent<ofNode>()){
                auto pos = node->getGlobalPosition();
                auto scale = node->getGlobalScale();
                mCachedBounds = ofRectangle( mOrigin.x + pos.x, mOrigin.y + pos.y, mSize.x * scale.x, mSize.y * scale.y );
            }
        }
        
        bool contains( const glm::vec2& point )const{ return mCachedBounds.inside(point); }
        const ofRectangle& getScreenBounds()const { return mCachedBounds; }
        
    private:
        
        EventStatus onUpdate(const IEventRef&){
            update();
            return EventStatus::SUCCESS;
        }
        
        Entity& mContext;
        ofRectangle mCachedBounds;
        glm::vec2 mSize;
        glm::vec2 mOrigin;
    };
    
    struct ScreenBoundsDebug {
        ScreenBoundsDebug(std::shared_ptr<ScreenBounds> comp):input(comp){}
        std::shared_ptr<ScreenBounds> input;
        void draw(){
            ofPushStyle();
            ofNoFill();
            auto matrix = glm::inverse(ofGetCurrentViewMatrix()) * ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_MODELVIEW);
            ofMultMatrix(glm::inverse(matrix));
            ofSetColor(255,255,0);
            ofDrawRectangle(input->getScreenBounds());
            ofMultMatrix(matrix);
            ofPopStyle();
        }
    };
    
}//end namespace mediasystem
