//
//  MediaBase.hpp
//  WallTest
//
//  Created by Michael Allison on 6/13/18.
//

#pragma once
#include "ofMain.h"
#include "mediasystem/core/Entity.hpp"

namespace mediasystem {
    
    class MediaBase {
    public:
        MediaBase(Entity& context):mContext(context),mNode(context.getComponent<ofNode>()){}
        virtual ~MediaBase() = default;
        virtual void init() = 0;
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void debugDraw() = 0;
        virtual bool isInit() const = 0;
        virtual glm::vec2 getMediaSize() = 0;
    protected:
        Entity& mContext;
        std::weak_ptr<ofNode> mNode;
    };
    
}//end namespace mediasystem
