//
//  ImageView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include "ofMain.h"
#include "mediasystem/behaviors/ViewBase.h"

namespace mediasystem {
    
    class ImageView : public ViewBase {
    public:
        
        ImageView(SceneBase* scene);
        ImageView(SceneBase* scene, const std::filesystem::path& path);
        ImageView(SceneBase* scene, int width, int height, ofImageType type);
        ~ImageView();
        
        virtual void init()override;
        virtual void draw()override;
        virtual void debugDraw()override;

        virtual void start()override{}
        virtual void stop()override{}
        virtual void update()override{}
        
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
        
    };
    
}//end namespace mediasystem
