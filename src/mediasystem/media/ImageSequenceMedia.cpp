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
    
    ImageSequenceMedia::ImageSequenceMedia(std::filesystem::path path, ImgSeqType type, float fps, Playable::Options options ):
        mType(type)
    {
        switch(type){
            case STATIC_SEQ:
            {
                mSequence = std::make_shared<ImageSequence>(std::move(path), fps, std::move(options));
            }break;
            case STREAMING_SEQ:
            {
                mSequence = std::make_shared<StreamingImageSequence>(std::move(path), fps, std::move(options));
            }break;
        }
    }
    
    ImageSequenceMedia::~ImageSequenceMedia()
    {
        
    }
    
    void ImageSequenceMedia::bind()
    {
        if(auto tex = mSequence->getCurrentTexture()){
            tex->bind();
        }
    }
    
    void ImageSequenceMedia::unbind()
    {
        if(auto tex = mSequence->getCurrentTexture()){
            tex->unbind();
        }
    }
    
    void ImageSequenceMedia::play()
    {
        mSequence->play();
    }
    
    void ImageSequenceMedia::stop()
    {
        mSequence->stop();
    }
    
    void ImageSequenceMedia::pause()
    {
        mSequence->pause();
    }
    
    void ImageSequenceMedia::unpause()
    {
        mSequence->play();
    }
    
    bool ImageSequenceMedia::isPlaying() const
    {
        return mSequence->isPlaying();
    }
    
    bool ImageSequenceMedia::isPaused() const
    {
        return mSequence->isPaused();
    }
    
    void ImageSequenceMedia::load()
    {
        if(!mSequence->isInit()){
            mSequence->init();
        }
    }
    
    bool ImageSequenceMedia::isLoaded()const
    {
        return mSequence->isInit();
    }
    
    void ImageSequenceMedia::update(size_t frame, float elapsedTime, float lastFrameTime)
    {
        mSequence->step(lastFrameTime);
    }
    
    void ImageSequenceMedia::setLoop(bool loop)
    {
        return mSequence->setLoop(loop);
    }
    
    bool ImageSequenceMedia::isLooping() const
    {
        return mSequence->isLooping();
    }
    
    glm::vec2 ImageSequenceMedia::getMediaSize() const
    {
        return mSequence->getSize();
    }
    
}//end namespace mediasystem
