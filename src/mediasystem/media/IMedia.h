//
//  MediaBase.hpp
//  WallTest
//
//  Created by Michael Allison on 6/13/18.
//

#pragma once
#include "ofMain.h"
#include "mediasystem/rendering/MeshRenderer.h"

namespace mediasystem {
    
    using MediaHandle = std::weak_ptr<class IMedia>;
    using PlayableMediaHandle = std::weak_ptr<class IPlayableMedia>;

    class IMedia : public IMaterial {
    public:
        virtual ~IMedia() = default;
        virtual glm::vec2 getMediaSize()const = 0;
        virtual bool isPlayable() const { return false; }
        virtual void load() = 0;
        virtual bool isLoaded() const = 0;
    };
    
    class IPlayableMedia : public IMedia {
    public:
        virtual ~IPlayableMedia() = default;
        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void unpause() = 0;
        virtual bool isPlaying() const = 0;
        virtual bool isPaused() const = 0;
        virtual void setLoop(bool loop) = 0;
        virtual bool isLooping()const = 0;
        virtual bool isPlayable() const { return true; }
        virtual void update(size_t frame, float elapedTime, float lastFrameTime) = 0;
    };
    
}//end namespace mediasystem
