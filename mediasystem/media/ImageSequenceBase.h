#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <vector>
#include <unordered_map>
#include "ofMain.h"
#include "mediasystem/util/Playable.h"

namespace mediasystem {

    class ImageSequenceBase : public Playable {
    public:
        
        ImageSequenceBase( float fps, const Playable::Options& options = Playable::Options());
        virtual ~ImageSequenceBase() = default;

        virtual void initPaths(const std::filesystem::path& imageSequenceDir);
    
        inline glm::vec2 getSize() const { return mSize; }
        inline void setSize( const glm::vec2& size ) { mSize = size; }
        inline void setSize( float w, float h ) { setSize(glm::vec2(w,h)); }

    protected:
        
        virtual ofTexture* getCurrentTexture() = 0;

        static void parseSource(const std::filesystem::path& path, std::vector<std::filesystem::path>& imageFiles, const std::vector<std::filesystem::path> allowedExt = { ".png", ".jpg", ".gif", ".JPG", ".PNG", ".jpeg", ".JPEG" });
        static void loadImageData(const std::filesystem::path& data, ofImage& surface);
        
        glm::vec2 mSize;
        std::vector<std::filesystem::path> mSeqPaths;
        
    };

}//end namespace mediasystem
