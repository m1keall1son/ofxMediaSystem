#pragma once

#include "ofMain.h"
#include "mediasystem/media/ImageSequenceMediaBase.h"

namespace mediasystem {
    
    class ImageSequenceMedia : public ImageSequenceMediaBase {
	public:

        ImageSequenceMedia(Entity& entity, float fps = 30.f, const Playable::Options& options = Playable::Options());
        ImageSequenceMedia(Entity& entity, const std::filesystem::path& source, float framerate, const Playable::Options& options = Playable::Options());
        
        void init()override;
        bool isInit()const override;
        void debugDraw(const ofRectangle& area, float fontsize)override;
        void bind()override;
        void unbind()override;
        
        glm::vec2 getMediaSize()const override;
        
	protected:
        
        ofTexture* getCurrentTexture() override;
        std::vector<ofImage> mImages;
        
	};
    
}//end namespace mediasystem
