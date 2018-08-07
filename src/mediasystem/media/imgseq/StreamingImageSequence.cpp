//
//  StreamingImageSequence.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/26/18.
//

#include "StreamingImageSequence.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {
    
    bool Frame::operator==(const Frame& rhs)
    {
        return serial == rhs.serial && frame == rhs.frame;
    }
    
    bool Frame::operator!=(const Frame& rhs)
    {
        return !operator==(rhs);
    }
    
    bool Frame::operator<(const Frame& rhs)
    {
        return frame < rhs.frame;
    }
    
    bool Frame::operator<=(const Frame& rhs)
    {
        return frame <= rhs.frame;
    }
    
    bool Frame::operator>(const Frame& rhs)
    {
        return frame > rhs.frame;
    }
    
    bool Frame::operator>=(const Frame& rhs)
    {
        return frame >= rhs.frame;
    }
    
    void StreamingImageSequence::initPaths(const std::filesystem::path& imageSequenceDir)
    {
        ImageSequenceBase::initPaths(imageSequenceDir);
        mLoadingState = mState;
    }
    
    void StreamingImageSequence::step(double timestep)
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
    
    StreamingImageSequence::~StreamingImageSequence()
    {
        stopDataLoadingThread();
    }
    
    StreamingImageSequence::StreamingImageSequence(std::filesystem::path pathToImgsDir,  float framerate, Playable::Options options) :
        ImageSequenceBase(std::move(pathToImgsDir), framerate, options)
    {
        mLoadingState = mState;
        initPaths(mImagesDir);
        startDataLoadingThread();
    }
    
    void StreamingImageSequence::init()
    {
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
    
    glm::vec2 StreamingImageSequence::getSize()const
    {
        return glm::vec2(mCurrentImage.getWidth(), mCurrentImage.getHeight());
    }
        
    void StreamingImageSequence::play(){
        if(isPlaying() && !isPaused()) return;
        Playable::play();
        flushQueue();
        ofLogNotice() << mSeqPaths[0].string() << " starting!";
    }

    ofTexture* StreamingImageSequence::getCurrentTexture()
    {
        if(!mCurrentImage.isAllocated()) return nullptr;
        return &mCurrentImage;
    }
    
    void StreamingImageSequence::setLoop(const bool flag)
    {
        mState.setLoop(flag);
        flushQueue();
    }
    
    void StreamingImageSequence::setReverse(const bool flag)
    {
        mState.setReverse(flag);
        flushQueue();
    }
    
    void StreamingImageSequence::setBoomarang(const bool flag)
    {
        mState.setBoomarang(flag);
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequence::setLoopPoints( uint32_t in, uint32_t out )
    {
        mState.loopPointIn = in;
        mState.loopPointOut = out;
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequence::setLoopPointIn( uint32_t frame )
    {
        mState.loopPointOut = frame;
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequence::setLoopPointOut( uint32_t frame )
    {
        mState.loopPointOut = frame;
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequence::setLoop( const bool flag, uint32_t inFrame, uint32_t outFrame )
    {
        mState.setLoop(flag);
        mState.loopPointIn = inFrame;
        mState.loopPointOut = outFrame;
        flushQueue();
    }
    
    void StreamingImageSequence::flushQueue()
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
    
    void StreamingImageSequence::startDataLoadingThread()
    {
        if(!mDataLoadingThreadRunning && !mDataLoadingThread){
            mDataLoadingThreadRunning = true;
            mDataLoadingThread.reset( new std::thread(&StreamingImageSequence::dataLoadingThread, this));
        }
    }
    
    void StreamingImageSequence::stopDataLoadingThread()
    {
        if(mDataLoadingThreadRunning && mDataLoadingThread){
            mDataLoadingThreadRunning = false;
            mQueue.abort();
            mDataLoadingThread->join();
            mDataLoadingThread.reset();
        }
    }
    
    void StreamingImageSequence::dataLoadingThread()
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
    
}//end namespace mediasystem
