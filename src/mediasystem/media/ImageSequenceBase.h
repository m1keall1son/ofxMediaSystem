#pragma once

#include "ofMain.h"
#include "mediasystem/util/Playable.h"

namespace mediasystem {

    class ImageSequenceBase : public Playable {
    public:
        
        ImageSequenceBase( float fps, const Playable::Options& options = Playable::Options());
        virtual ~ImageSequenceBase() = default;

        virtual void initPaths(const std::filesystem::path& imageSequenceDir);

    protected:
        
        virtual ofTexture* getCurrentTexture() = 0;

        static void parseSource(const std::filesystem::path& path, std::vector<std::filesystem::path>& imageFiles, const std::vector<std::filesystem::path> allowedExt = { ".png", ".jpg", ".gif", ".JPG", ".PNG", ".jpeg", ".JPEG" });
        static void loadImageData(const std::filesystem::path& data, ofImage& surface);
        
        std::vector<std::filesystem::path> mSeqPaths;
        
    };

}//end namespace mediasystem
