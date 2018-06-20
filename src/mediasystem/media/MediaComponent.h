//
//  MediaComponent.hpp
//  WallTest
//
//  Created by Michael Allison on 6/19/18.
//

#pragma once

#include <memory>
#include "ofMain.h"
#include "mediasystem/media/MediaBase.h"

namespace mediasystem {
    
    using MediaComponentHandle = std::weak_ptr<class MediaComponent>;
    
    class MediaComponent {
    public:
        
        enum RectMode { RECT_MODE_CENTER, RECT_MODE_TOP_LEFT };
        enum TextureMode { RECTANGLE, NORMALIZED };
        
        MediaComponent(std::shared_ptr<MediaBase> media, RectMode mode = RECT_MODE_TOP_LEFT, TextureMode tc = RECTANGLE );
        
        MediaComponent(std::shared_ptr<MediaBase> media, float width, float height, RectMode mode = RECT_MODE_TOP_LEFT, TextureMode tc = RECTANGLE);
        
        ~MediaComponent() = default;
        
        void draw();
        //void debugDraw();
        
        glm::vec2 getMediaSize();
        glm::vec2 getSize() const;
        ofRectangle getRect() const;
        void setSize(float w, float h);
        inline void setSize(const glm::vec2& size ){ setSize(size.x, size.y);}
        
        void setRectMode(RectMode mode);
        RectMode getRectMode()const;
        
        inline std::shared_ptr<MediaBase> getMedia(){ return mMedia; }
        
    protected:
        
        void setRect(float w, float h, float mediaW, float mediaH);
    
        RectMode mRectMode;
        TextureMode mTexMode;
        ofMesh mRect;
        ofRectangle mDebugRect;
        std::shared_ptr<MediaBase> mMedia{nullptr};
    };
    
}//end namespace mediasystem

