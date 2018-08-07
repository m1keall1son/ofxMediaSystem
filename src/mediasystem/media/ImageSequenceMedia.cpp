//
//  ImageSequenceViewBase.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#include "ImageSequenceMedia.h"
#include "StreamingImageSequence.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    ImageSequenceMedia::ImageSequenceMedia(std::filesystem::path path, float fps, Playable::Options options ):
        ImageSequence(std::move(path),fps,std::move(options))
    {}

    
    void ImageSequenceMedia::bind()
    {
        if(auto tex = getCurrentTexture()){
            tex->bind();
        }
    }
    
    void ImageSequenceMedia::unbind()
    {
        if(auto tex = getCurrentTexture()){
            tex->unbind();
        }
    }
    
    void ImageSequenceMedia::load()
    {
        if(!isInit()){
            init();
        }
    }
    
    bool ImageSequenceMedia::isLoaded()const
    {
        return isInit();
    }
    
    void ImageSequenceMedia::update(size_t frame, float elapsedTime, float lastFrameTime)
    {
        step(lastFrameTime);
    }

    glm::vec2 ImageSequenceMedia::getMediaSize() const
    {
        return ImageSequence::getSize();
    }
    
    /////////////
    
    StreamingImageSequenceMedia::StreamingImageSequenceMedia(std::filesystem::path path, float fps, Playable::Options options ):
        StreamingImageSequence(std::move(path),fps,std::move(options))
    {}
    
    void StreamingImageSequenceMedia::bind()
    {
        if(auto tex = getCurrentTexture()){
            tex->bind();
        }
    }
    
    void StreamingImageSequenceMedia::unbind()
    {
        if(auto tex = getCurrentTexture()){
            tex->unbind();
        }
    }
    
    void StreamingImageSequenceMedia::load()
    {
        if(!isInit()){
            init();
        }
    }
    
    bool StreamingImageSequenceMedia::isLoaded()const
    {
        return isInit();
    }
    
    void StreamingImageSequenceMedia::update(size_t frame, float elapsedTime, float lastFrameTime)
    {
        step(lastFrameTime);
    }
    
    glm::vec2 StreamingImageSequenceMedia::getMediaSize() const
    {
        return getSize();
    }
    
}//end namespace mediasystem
