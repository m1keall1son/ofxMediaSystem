
#include "ImageSequence.h"
#include <algorithm>
#include "mediasystem/util/Log.h"
#include "mediasystem/util/LockingQueue.hpp"
#include "ofMain.h"

using namespace std;
using namespace std::chrono;

namespace mediasystem {
    
    ImageSequenceBase::ImageSequenceBase(std::filesystem::path&& pathToImgDir, float fps, const Playable<uint32_t>::Options& options) :
        Playable<uint32_t>(0),
        mImagesDir(std::move(pathToImgDir)),
        mFramerate(fps)
    {
        mInvFps = 1.f / mFramerate;
        parseSource(pathToImgDir, mSeqPaths);
        Playable<uint32_t>::init(mSeqPaths.size()-1,options);
    }
    
    void ImageSequenceBase::update(float amount){
        auto cur = amount + mElapsedFrameTime;
        if (cur >= mInvFps) {
            mElapsedFrameTime = 0.f;
            step(1);
        }else{
            mElapsedFrameTime  = cur;
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
            load();
        }
        
        void load()override{
            if(!mImagesDir.empty()){
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
            return !mImages.empty() ? &mImages[getCurrentPosition() < mImages.size() ? getCurrentPosition() : mImages.size()-1].getTexture() : nullptr;
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
            startDataLoadingThread();
            load();
        }
        
        ~StreamingImpl()
        {
            stopDataLoadingThread();
        }
        
        void step(uint32_t frames) override
        {
            if ((mState.isPlaying()) && !(mState.isPaused())) {
                
                //check delay
                if(mDelayElapsed >= mDelay){
                    if(mWaitingOnDelay){
                        if(mOnStartAfterDelay)
                            mOnStartAfterDelay();
                        mWaitingOnDelay = false;
                    }
                    
                    Frame retrieve;
                    
                    auto prevPosition = mState.currentPosition;
                    auto prevReverse = mState.isReversed();
                    
                    auto expectedState = Playable<uint32_t>::State::advance(mState,frames);
                    
                    while(true){
                        auto success = mQueue.tryPop(retrieve);
                        if(success){
                            
                            if(retrieve.serial != mSerial) //pop until we find one thats current
                                continue;
                            
                            if(retrieve.img){
                                if(retrieve.frame != expectedState.currentPosition){
                                    //this should never happen
                                    MS_LOG_ERROR("popped an out of order frame off the queue, should have been " << expectedState.currentPosition << "but received " << retrieve.frame);
                                }
                                mState.currentPosition = retrieve.frame;
                                mCurrentImage.allocate(retrieve.img->getPixels());
                                
                                if(retrieve.finished){
                                    ofLogNotice() << mSeqPaths[0].string() << " finished!";
                                    mState.setFinished();
                                    ImageSequenceBase::stop();
                                    if(mOnFinishFn)
                                        mOnFinishFn();
                                    return;
                                }
                                
                                if(mOnUpdateFn)
                                    mOnUpdateFn();
                                
                                //check loop
                                if(mState.isLooping()){
                                    if(mState.isPalindrome()){
                                        if(mState.isReversed() != prevReverse){
                                            if(mOnLoopedFn)
                                                mOnLoopedFn();
                                            resetKeyFrames();
                                        }
                                    }else{
                                        if(mState.isReversed()){
                                            if(mState.currentPosition > prevPosition){
                                                if(mOnLoopedFn)
                                                    mOnLoopedFn();
                                                resetKeyFrames();
                                            }
                                        }else{
                                            if(mState.currentPosition < prevPosition){
                                                if(mOnLoopedFn)
                                                    mOnLoopedFn();
                                                resetKeyFrames();
                                            }
                                        }
                                    }
                                }
                                
                                //check keyframes
                                if(!mKeyFrames.empty() && mCurrentKeyFrame != mKeyFrames.end()){
                                    if(mState.isReversed()){
                                        if( mState.currentPosition <= mCurrentKeyFrame->position){
                                            mCurrentKeyFrame->callback(mState);
                                            if(mCurrentKeyFrame == mKeyFrames.begin()){
                                                mCurrentKeyFrame = mKeyFrames.end();
                                            }else{
                                                auto it = --mCurrentKeyFrame;
                                                bool done = false;
                                                while(!done){
                                                    if(mState.currentPosition <= it->position){
                                                        auto tmp = mState;
                                                        mCurrentKeyFrame->callback(mState);
                                                        if(tmp != mState){
                                                            mState = tmp;
                                                            flushQueue();
                                                        }
                                                        if(mCurrentKeyFrame == mKeyFrames.begin()){
                                                            done = true;
                                                        }else{
                                                            mCurrentKeyFrame = --it;
                                                        }
                                                    }else{
                                                        done = true;
                                                    }
                                                }
                                            }
                                        }
                                    }else{
                                        if(mState.currentPosition >= mCurrentKeyFrame->position){
                                            auto tmp = mState;
                                            mCurrentKeyFrame->callback(mState);
                                            if(tmp != mState){
                                                mState = tmp;
                                                flushQueue();
                                            }
                                            auto it = ++mCurrentKeyFrame;
                                            while(it != mKeyFrames.end()){
                                                if(mState.currentPosition >= it->position){
                                                    auto tmp = mState;
                                                    mCurrentKeyFrame->callback(mState);
                                                    if(tmp != mState){
                                                        mState = tmp;
                                                        flushQueue();
                                                    }
                                                    mCurrentKeyFrame = ++it;
                                                }else{
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                
                            }else{
                                MS_LOG_WARNING("stream failed to retrieve an image from new frame! expected frame: " << expectedState.currentPosition << " current frame: " << mState.currentPosition);
                            }
                        }else{
                            MS_LOG_WARNING("stream failed to acquire new frame! expected frame: " << expectedState.currentPosition << " current frame: " << mState.currentPosition);
                            ++mMissedFrames;
                        }
                        break;
                    }
                    
                }else{
                    mDelayElapsed += frames;
                }
            }
        }
        
        void play()override{
            if(isPlaying() && !isPaused()) return;
            Playable::play();
            flushQueue();
            ofLogNotice() << mSeqPaths[0].string() << " starting!";
        }
        
        void load()override{
            if(!mImagesDir.empty()){
                mCurrentImage.clear();
                reset();
                //load first frame
                ofImage img;
                img.setUseTexture(false);
                img.load(mSeqPaths[mState.currentPosition]);
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
        void setPalindrome(const bool flag = true)override{
            mState.setPalindrome(flag);
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
        
        void flushQueue()
        {
            {
                std::lock_guard<std::mutex> lock(mPlaystateMutex);
                mLoadingState = mState;
                ++mSerial;
                auto count = mQueue.size();
                Playable::State tmp = mState;
                for(size_t i = 0; i < count; i++){
                    tmp = Playable::State::advance(tmp, 1);
                    auto frame = mQueue.peek(i);
                    if(frame->frame == tmp.currentPosition){
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
                    mLoadingState = Playable::State::advance( mLoadingState, 1 );
                    curFrame = mLoadingState.currentPosition;
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
