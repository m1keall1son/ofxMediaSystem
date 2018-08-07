
#include "ImageSequence.h"
#include <algorithm>
#include "mediasystem/util/Log.h"
#include "mediasystem/util/LockingQueue.hpp"
#include "ofMain.h"

using namespace std;
using namespace std::chrono;

namespace mediasystem {
    
    ImageSequenceBase::ImageSequenceBase(std::filesystem::path&& pathToImgDir, float fps, const Playable::Options& options) :
    Playable(fps, options),
    mImagesDir(std::move(pathToImgDir))
    {}
    
    void ImageSequenceBase::setImgDir(std::filesystem::path&& pathToImgDir)
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
    
    void ImageSequenceBase::bind()
    {
        if(auto tex = getCurrentTexture()){
            tex->bind(mTextureLocation);
        }
    }
    
    void ImageSequenceBase::unbind()
    {
        if(auto tex = getCurrentTexture()){
            tex->unbind(mTextureLocation);
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
    
    class PreloadImpl : public ImageSequenceBase {
    public:
        
        PreloadImpl(std::filesystem::path&& pathToImgDir, float fps, const Playable::Options& options): ImageSequenceBase(std::move(pathToImgDir), fps, std::move(options))
        {
            init();
        }
        
        void init()override{
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
        
        ofTexture* getCurrentTexture()override {
            return !mImages.empty() ? &mImages[getCurrentFrame() < mImages.size() ? getCurrentFrame() : mImages.size()-1].getTexture() : nullptr;
        }
        
        glm::vec2 getSize() const override {
            if(!mImages.empty()){
                return glm::vec2(mImages.front().getWidth(), mImages.front().getHeight());
            }
            return glm::vec2(0);
        }
        
    private:
        std::vector<ofImage> mImages;
    };
    
    
    struct Frame {
        Frame():img(nullptr),frame(0),serial(0),finished(false){}
        Frame(uint32_t f, const std::shared_ptr<ofImage>& d, uint32_t s, bool fin):img(d),frame(f),serial(s),finished(fin){}
        std::shared_ptr<ofImage> img;
        uint32_t frame;
        uint32_t serial;
        bool finished;
        bool operator==(const Frame& rhs)
        {
            return serial == rhs.serial && frame == rhs.frame;
        }
        
        bool operator!=(const Frame& rhs)
        {
            return !operator==(rhs);
        }
        
        bool operator<(const Frame& rhs)
        {
            return frame < rhs.frame;
        }
        
        bool operator<=(const Frame& rhs)
        {
            return frame <= rhs.frame;
        }
        
        bool operator>(const Frame& rhs)
        {
            return frame > rhs.frame;
        }
        
        bool operator>=(const Frame& rhs)
        {
            return frame >= rhs.frame;
        }
    };
    
    class StreamingImpl : public ImageSequenceBase {
    public:
        
        StreamingImpl(std::filesystem::path&& pathToImgsDir,  float framerate, const Playable::Options& options) :
            ImageSequenceBase(std::move(pathToImgsDir), framerate, options)
        {
            mLoadingState = mState;
            initPaths(mImagesDir);
            startDataLoadingThread();
            init();
        }
        
        ~StreamingImpl()
        {
            stopDataLoadingThread();
        }
        
        void step(double timestep) override
        {
            auto cur = timestep + mElapsedTime;
            auto rate = (1.f/mFramerate);
            if ((mState.isPlaying()) && !(mState.isPaused()) && cur >= rate) {
                mElapsedTime = 0.0;
                Frame retrieve;
                
                auto expectedState = Playable::State::advance(mState);
                
                while(true){
                    auto success = mQueue.tryPop(retrieve);
                    if(success){
                        
                        if(retrieve.serial != mSerial) //pop until we find one thats current
                            continue;
                        
                        if(retrieve.img){
                            if(retrieve.frame != expectedState.currentFrame){
                                //this should never happen
                                MS_LOG_ERROR("popped an out of order frame off the queue, should have been " << expectedState.currentFrame << "but received " << retrieve.frame);
                            }
                            mState.currentFrame = retrieve.frame;
                            mCurrentImage.allocate(retrieve.img->getPixels());
                            
                            if(retrieve.finished){
                                ofLogNotice() << mSeqPaths[0].string() << " finished!";
                                mState.setFinished();
                                ImageSequenceBase::stop();
                            }
                            
                            auto keyframe = mKeyFrames.find(mState.currentFrame);
                            if(keyframe != mKeyFrames.end()){
                                auto tmp = mState;
                                keyframe->second(tmp);
                                if(tmp != mState){
                                    mState = tmp;
                                    flushQueue();
                                }
                            }
                            
                        }else{
                            MS_LOG_WARNING("stream failed to retrieve an image from new frame! expected frame: " << expectedState.currentFrame << " current frame: " << mState.currentFrame);
                        }
                    }else{
                        MS_LOG_WARNING("stream failed to acquire new frame! expected frame: " << expectedState.currentFrame << " current frame: " << mState.currentFrame);
                        ++mMissedFrames;
                    }
                    break;
                }
            }else{
                mElapsedTime += timestep;
            }
        }
        
        void play()override{
            if(isPlaying() && !isPaused()) return;
            Playable::play();
            flushQueue();
            ofLogNotice() << mSeqPaths[0].string() << " starting!";
        }
        
        void init()override{
            if(!mImagesDir.empty()){
                mCurrentImage.clear();
                reset();
                //load first frame
                ofImage img;
                img.setUseTexture(false);
                img.load(mSeqPaths[mState.currentFrame]);
                mCurrentImage.allocate(img.getPixels());
                mIsInit = true;
            }
        }
        
        ofTexture* getCurrentTexture() override{
            if(!mCurrentImage.isAllocated()) return nullptr;
            return &mCurrentImage;
        }
        
        glm::vec2 getSize()const override{
            return glm::vec2(mCurrentImage.getWidth(), mCurrentImage.getHeight());
        }
        
        void setLoop( const bool flag = true )override{
            mState.setLoop(flag);
            flushQueue();
        }
        void setReverse(const bool flag = true)override{
            mState.setReverse(flag);
            flushQueue();
        }
        void setBoomarang(const bool flag = true)override{
            mState.setBoomarang(flag);
            if(mState.isLooping())
                flushQueue();
        }
        void setLoopPointIn( uint32_t frame )override{
            mState.loopPointOut = frame;
            if(mState.isLooping())
                flushQueue();
        }
        void setLoopPointOut( uint32_t frame )override{
            mState.loopPointOut = frame;
            if(mState.isLooping())
                flushQueue();
        }
        
        void setLoopPoints( uint32_t inFrame, uint32_t outFrame )override{
            mState.loopPointIn = inFrame;
            mState.loopPointOut = outFrame;
            if(mState.isLooping())
                flushQueue();
        }
        void setLoop( const bool flag, uint32_t inFrame, uint32_t outFrame ){
            mState.setLoop(flag);
            mState.loopPointIn = inFrame;
            mState.loopPointOut = outFrame;
            flushQueue();
        }
        
    private:
        
        void initPaths(const std::filesystem::path& imageSequenceDir)override
        {
            ImageSequenceBase::initPaths(imageSequenceDir);
            mLoadingState = mState;
        }

        void flushQueue()
        {
            {
                std::lock_guard<std::mutex> lock(mPlaystateMutex);
                mLoadingState = mState;
                ++mSerial;
                auto count = mQueue.size();
                Playable::State tmp = mState;
                for(size_t i = 0; i < count; i++){
                    tmp = Playable::State::advance(tmp);
                    auto frame = mQueue.peek(i);
                    if(frame->frame == tmp.currentFrame){
                        frame->serial = mSerial;
                        frame->finished = tmp.isFinished();
                        mLoadingState = tmp;
                    }else{
                        mQueue.flush(i);
                        break;
                    }
                }
            }
        }
        
        void startDataLoadingThread()
        {
            if(!mDataLoadingThreadRunning && !mDataLoadingThread){
                mDataLoadingThreadRunning = true;
                mDataLoadingThread.reset( new std::thread(&StreamingImpl::dataLoadingThread, this));
            }
        }
        
        void stopDataLoadingThread()
        {
            if(mDataLoadingThreadRunning && mDataLoadingThread){
                mDataLoadingThreadRunning = false;
                mQueue.abort();
                mDataLoadingThread->join();
                mDataLoadingThread.reset();
            }
        }
        
        void dataLoadingThread()
        {
            while(mDataLoadingThreadRunning){
                uint32_t curFrame = 0;
                uint32_t serial = 0;
                bool fin = false;
                {
                    std::lock_guard<std::mutex> lock(mPlaystateMutex);
                    mLoadingState = Playable::State::advance( mLoadingState );
                    curFrame = mLoadingState.currentFrame;
                    serial = mSerial;
                    fin = mLoadingState.isFinished();
                    if(fin){
                        mLoadingState.setFinished(false);
                    }
                }
                auto img = std::make_shared<ofImage>();
                img->setUseTexture(false);
                img->load(mSeqPaths[curFrame]);
                mQueue.push({curFrame, img, serial, fin});
            }
        }
        
        std::mutex mPlaystateMutex;
        uint32_t mSerial{0};
        Playable::State mLoadingState;
        LockingQueue<Frame,3> mQueue;
        ofTexture mCurrentImage;
        
        std::unique_ptr<std::thread> mDataLoadingThread{nullptr};
        bool mDataLoadingThreadRunning{false};
        
        //debug
        uint32_t mMissedFrames{0};
        
    };
    
    ImageSequence::ImageSequence(std::filesystem::path pathToImgDir, float fps, const ImageSequence::Options& options):
        mType(options.mType)
    {
        switch(options.mType){
            case SEQ_PRELOAD:
                mImpl.reset(new PreloadImpl(std::move(pathToImgDir), fps, options.mPlayableOptions));
                break;
            case SEQ_DISK_STREAMING:
                mImpl.reset(new StreamingImpl(std::move(pathToImgDir), fps,  options.mPlayableOptions));
                break;
        }
    }

}//end namespace mediasystem
