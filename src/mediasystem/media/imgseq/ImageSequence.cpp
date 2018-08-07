
#include "ImageSequence.h"
#include <algorithm>
#include "mediasystem/util/Log.h"
#include "ofMain.h"

using namespace std;
using namespace std::chrono;

namespace mediasystem {
    
    ImageSequenceBase::ImageSequenceBase(std::filesystem::path&& pathToImgDir, float fps, const Playable::Options& options) :
        Playable(fps, options),
        mImagesDir(std::move(pathToImgDir))
    {}
    
    void ImageSequenceBase::setImgDir(std::filesystem::path pathToImgDir)
    {
        mImagesDir = std::move(pathToImgDir);
        mIsInit = false;
    }

    void ImageSequenceBase::initPaths( const std::filesystem::path& imageSequenceDir)
    {
        parseSource(imageSequenceDir, mSeqPaths);
        mState.totalFrames = mSeqPaths.size();
        if(mState.loopPointIn == std::numeric_limits<uint32_t>::max() || mState.loopPointOut == std::numeric_limits<uint32_t>::max()){
            mState.loopPointIn = 0;
            mState.loopPointOut = mState.totalFrames-1;
        }
    }
    
    void ImageSequenceBase::parseSource(const std::filesystem::path & path, std::vector<std::filesystem::path>& imageFiles, const std::vector<std::filesystem::path> allowedExt )
    {
        imageFiles.clear();

        if (std::filesystem::exists(path)) {
            std::filesystem::directory_iterator end_itr;
            for (std::filesystem::directory_iterator itr(path); itr != end_itr; ++itr)
            {
                if (!std::filesystem::is_directory(itr->status()))
                {
                    auto found = std::find(allowedExt.begin(), allowedExt.end(), itr->path().extension());
                    if(found != allowedExt.end()){
                        imageFiles.push_back(itr->path());
                    }
                }
            }
        }
        else {
            MS_LOG_ERROR("Could not find image squence file path: " + path.string());
        }
        std::stable_sort(imageFiles.begin(), imageFiles.end());
    }
    
    ImageSequence::ImageSequence(std::filesystem::path pathToImgDir, float fps, Playable::Options options):
        ImageSequenceBase(std::move(pathToImgDir), fps, std::move(options))
    {
    }
    
    void ImageSequence::init()
    {
        if(!mImagesDir.empty()){
            initPaths(mImagesDir);
            mImages.clear();
            for ( auto & path : mSeqPaths ) {
                ofImage surface;
                surface.load(path);
                mImages.push_back(surface);
            }
            reset();
            mIsInit = true;
        }
    }
    
    ofTexture* ImageSequence::getCurrentTexture()
    {
        return !mImages.empty() ? &mImages[getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1].getTexture() : nullptr;
    }
    
    glm::vec2 ImageSequence::getSize() const
    {
        if(!mImages.empty()){
            return glm::vec2(mImages.front().getWidth(), mImages.front().getHeight());
        }
        return glm::vec2(0);
    }

}//end namespace mediasystem
