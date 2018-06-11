#pragma once

#include "mediasystem/media/ImageSequenceViewBase.h"

namespace mediasystem {
    
    class ImageSequenceView : public ImageSequenceViewBase {
	public:

        ImageSequenceView(SceneBase* scene, float fps = 30.f, const Playable::Options& options = Playable::Options());
        ImageSequenceView(SceneBase* scene, const std::filesystem::path& source, float framerate, const Playable::Options& options = Playable::Options());
        
		void init()override;
        void draw()override;
        void debugDraw()override;
        
	protected:

        ofTexture* getCurrentTexture() override;
        std::vector<ofImage> mImages;
        
	};
    
}//end namespace mediasystem
