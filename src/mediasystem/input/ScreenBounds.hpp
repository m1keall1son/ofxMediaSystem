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
            mBounds(rect),
            mSize(rect.width, rect.height)
        {}
        
        void update(){
            if(auto node = mContext.getComponent<ofNode>()){
                auto pos = node->getGlobalPosition();
                auto scale = node->getGlobalScale();
                mBounds = ofRectangle( pos.x, pos.y, mSize.x * scale.x, mSize.y * scale.y );
            }
        }
        
        bool contains( const glm::vec2& point )const{ return mBounds.inside(point); }
        const ofRectangle& getScreenBounds()const { return mBounds; }
        
    private:
        Entity& mContext;
        ofRectangle mBounds;
        glm::vec2 mSize;
    };
    
}//end namespace mediasystem
