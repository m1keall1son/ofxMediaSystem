//
//  ImageView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include "ofMain.h"
#include "mediasystem/media/MediaBase.h"

namespace mediasystem {
        
    class ImageMedia : public MediaBase {
    public:
        
        ImageMedia(Entity& context, const std::filesystem::path& path);
        ImageMedia(Entity& context, int width, int height, ofImageType type);
        virtual ~ImageMedia() = default;
        
        void init()override;
        virtual void bind()override;
        virtual void unbind()override;
        virtual bool isInit()const override;
        virtual void debugDraw(const ofRectangle& area, float fontsize)override;
        
        glm::vec2 getMediaSize()const override;
        
        virtual void load(const std::filesystem::path& path);
        inline ofImage& getImage(){ return mImage; }
        inline glm::vec2 getSize() const { return glm::vec2(mWidth,mHeight); }
        inline void setSize( const glm::vec2& size ) { setSize(size.x, size.y); }
        inline void setSize( float w, float h ) { mWidth = w; mHeight = h; }

    private:
        
        
        std::filesystem::path mFilename;
        int mWidth;
        int mHeight;
        ofImageType mType;
        ofImage mImage;
        bool mIsInit{false};
        
    };
    
}//end namespace mediasystem
