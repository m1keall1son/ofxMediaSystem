//
//  ImageSequenceViewBase.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#pragma once

#include "imgseq/ImageSequence.h"
#include "imgseq/StreamingImageSequence.h"
#include "IMedia.h"

namespace mediasystem {
    
    class ImageSequenceMedia : public IMedia, public ImageSequence {
    public:
        
        ImageSequenceMedia(std::filesystem::path path, float fps, Playable::Options options = Playable::Options() );
        
        void load()override;
        bool isLoaded()const override;
        
        void bind()override;
        void unbind()override;
        
        glm::vec2 getMediaSize() const override;

        void update(size_t frame, float elapsedTime, float lastFrameTime)override;
    };
    
    class StreamingImageSequenceMedia : public IMedia, public StreamingImageSequence {
    public:
        
        StreamingImageSequenceMedia(std::filesystem::path path, float fps, Playable::Options options = Playable::Options() );
        
        void load()override;
        bool isLoaded()const override;
        
        void bind()override;
        void unbind()override;
       
        glm::vec2 getMediaSize() const override;
        
        void update(size_t frame, float elapsedTime, float lastFrameTime)override;
    
    };
    
}//end namespace mediasystem
