//
//  ImageSequenceViewBase.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/7/18.
//

#pragma once

#include "ImageSequence.h"
#include "IMedia.h"

namespace mediasystem {
        
    class ImageSequenceMedia : public IPlayableMedia {
    public:
      
        enum ImgSeqType { STATIC_SEQ, STREAMING_SEQ };
        
        ImageSequenceMedia(std::filesystem::path path, ImgSeqType type, float fps, Playable::Options options = Playable::Options() );
        ~ImageSequenceMedia();
        
        void load()override;
        bool isLoaded()const override;
        void update(size_t frame, float elapsedTime, float lastFrameTime)override;
        
        void bind()override;
        void unbind()override;
        
        void play()override;
        void stop()override;
        void pause()override;
        void unpause()override;
        void setLoop(bool loop)override;
        bool isPlaying() const override;
        bool isPaused() const override;
        bool isLooping() const override;
        
        glm::vec2 getMediaSize() const override;
        
    protected:
        
        ImgSeqType mType{STATIC_SEQ};
        std::shared_ptr<ImageSequenceBase> mSequence;
        
    };
    
}//end namespace mediasystem
