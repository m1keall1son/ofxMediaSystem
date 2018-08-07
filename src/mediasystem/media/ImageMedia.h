//
//  ImageView.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include "ofMain.h"
#include "mediasystem/media/IMedia.h"

namespace mediasystem {
        
    class ImageMedia : public IMedia {
    public:
        
        ImageMedia(const std::filesystem::path& path);
        ImageMedia(int width, int height, ofImageType type);
        
        void load()override;
        inline bool isLoaded()const override { return mLoaded; }
        
        void bind()override;
        void unbind()override;

        glm::vec2 getMediaSize() const override;
        
        void reload(const std::filesystem::path& path);
                
        inline ofTexture& getTexture(){ return mImage.getTexture(); }
        inline ofImageType getImageType() const { return mType; }

    private:
        
        std::filesystem::path mFilename;
        int mWidth{0};
        int mHeight{0};
        ofImageType mType;
        ofImage mImage;
        bool mLoaded{false};
        
    };
    
}//end namespace mediasystem
