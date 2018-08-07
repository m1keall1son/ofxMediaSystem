//
//  ImageMedia.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "ImageMedia.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    ImageMedia::ImageMedia(const std::filesystem::path& path) :
        mFilename(path)
    {}
    
    ImageMedia::ImageMedia(int width, int height, ofImageType type) :
        mWidth(width),
        mHeight(height),
        mType(type)
    {}
    
    void ImageMedia::load()
    {
        mImage.clear();
        if(!mFilename.empty()){
            mImage.load(mFilename);
        }
        else{
            mImage.allocate(mWidth, mHeight, mType);
        }
        mWidth = mImage.getWidth();
        mHeight = mImage.getHeight();
        mType = mImage.getImageType();
        mLoaded = true;
    }
    
    glm::vec2 ImageMedia::getMediaSize() const
    {
        return glm::vec2(mWidth, mHeight);
    }
    
    void ImageMedia::bind()
    {
        mImage.bind();
    }
    
    void ImageMedia::unbind()
    {
        mImage.unbind();
    }
    
    void ImageMedia::reload(const std::filesystem::path& path)
    {
        mFilename = path;
        mLoaded = false;
    }
    
}//end namespace mediasystem
