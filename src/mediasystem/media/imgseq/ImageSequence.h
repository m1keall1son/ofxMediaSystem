#pragma once

#include "ofMain.h"
#include "mediasystem/util/Playable.h"

namespace mediasystem {

    class ImageSequenceBase : public Playable {
    public:
        
        ImageSequenceBase(std::filesystem::path&& pathToImgDir, float fps, const Playable::Options& options);
        virtual ~ImageSequenceBase() = default;

        void setImgDir(std::filesystem::path pathToImgDir);
        
        virtual void init() = 0;
        virtual ofTexture* getCurrentTexture() = 0;
        virtual glm::vec2 getSize() const  = 0;
        
        bool isInit() const { return mIsInit; }
        
    protected:
        
        virtual void initPaths(const std::filesystem::path& imageSequenceDir);
    
        static void parseSource(const std::filesystem::path& path, std::vector<std::filesystem::path>& imageFiles, const std::vector<std::filesystem::path> allowedExt = { ".png", ".jpg", ".gif", ".JPG", ".PNG", ".jpeg", ".JPEG" });
        
        std::filesystem::path mImagesDir;
        std::vector<std::filesystem::path> mSeqPaths;
        bool mIsInit{false};
    };
    
    class ImageSequence : public ImageSequenceBase {
    public:
        
        ImageSequence(std::filesystem::path pathToImgDir, float fps, Playable::Options options = Playable::Options());
        
        void init()override;
        ofTexture* getCurrentTexture()override;
        glm::vec2 getSize() const override;
        
    private:
        std::vector<ofImage> mImages;
    };

}//end namespace mediasystem
