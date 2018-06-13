//
//  RenderComponent.hpp
//  WallTest
//
//  Created by Michael Allison on 6/12/18.
//

#pragma once

#include <memory>
#include "ofMain.h"
#include "mediasystem/media/MediaBase.h"
#include "mediasystem/core/Entity.hpp"

namespace mediasystem {
    
    class Entity;
    
    using LayeredMediaComponentHandle = std::weak_ptr<class LayeredMediaComponent>;

    class LayeredMediaComponent {
    public:
        
        enum RectMode { RECT_MODE_CENTER, RECT_MODE_TOP_LEFT };
        
        LayeredMediaComponent(Entity& context, std::shared_ptr<MediaBase> media, RectMode mode = RECT_MODE_TOP_LEFT );
        LayeredMediaComponent(Entity& context, std::shared_ptr<MediaBase> media, float width, float height, RectMode mode = RECT_MODE_TOP_LEFT );

         ~LayeredMediaComponent() = default;
        
        void draw();
        void debugDraw();
        
        glm::mat4 getGlobalTransform();

        inline void setLayer(float layer){ mLayer = layer; }
        inline float getLayer() const { return mLayer; }
        inline void hide(){ mVisible = false; }
        inline void show(){ mVisible = true; }
        inline bool isVisible()const{ return mVisible; }
        
        inline void setColor(const ofFloatColor& color){ mColor = color; }
        inline const ofFloatColor& getColor()const{ return mColor; }

        inline void setAlpha(float alpha){ mAlpha = alpha; }
        inline float getAlpha() const { return mAlpha; }
        
        glm::vec2 getMediaSize();
        
    protected:
        
        void setRect(float w, float h, float mediaW, float mediaH);
        
        Entity& mContext;
        std::weak_ptr<ofNode> mNode;
        ofFloatColor mColor{1.,1.,1.};
        RectMode mRectMode;
        float mAlpha{1.f};
        float mLayer{0};
        bool mVisible{true};
        ofMesh mRect;
        std::shared_ptr<MediaBase> mMedia{nullptr};
        
    };
    
}//end namespace mediasystem
