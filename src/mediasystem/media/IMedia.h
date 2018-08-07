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
        virtual void load() = 0;
        virtual bool isLoaded() const = 0;
        virtual bool isPlayable() const { return false; }
        virtual void update(size_t frame, float elapsedTime, float lastFrameTime){}
    };
    
}//end namespace mediasystem
