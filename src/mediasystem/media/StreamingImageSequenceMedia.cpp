//
//  StreamingImageSequence.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/26/18.
//

#include "StreamingImageSequenceMedia.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/core/Scene.h"

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
    
    void StreamingImageSequenceMedia::initPaths(const std::filesystem::path& imageSequenceDir)
    {
        ImageSequenceBase::initPaths(imageSequenceDir);
        mLoadingState = mState;
    }
    
    void StreamingImageSequenceMedia::step(double timestep)
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
    
    StreamingImageSequenceMedia::~StreamingImageSequenceMedia()
    {
        stopDataLoadingThread();
    }

    StreamingImageSequenceMedia::StreamingImageSequenceMedia(Entity& context, float framerate, const Playable::Options& options) : ImageSequenceMediaBase(context, framerate, options)
    {
        mLoadingState = mState;
        startDataLoadingThread();
    }
    
    StreamingImageSequenceMedia::StreamingImageSequenceMedia(Entity& context, const std::filesystem::path & source,  float framerate, const Playable::Options& options) : ImageSequenceMediaBase(context, framerate, options)
    {
        mLoadingState = mState;
        initPaths(source);
        startDataLoadingThread();
    }
    
    void StreamingImageSequenceMedia::init()
    {
        mCurrentImage.clear();
        reset();
        //load first frame
        ofImage img;
        img.setUseTexture(false);
        img.load(mSeqPaths[mState.currentFrame]);
        mCurrentImage.allocate(img.getPixels());
        mIsInit = true;
    }
    
    glm::vec2 StreamingImageSequenceMedia::getMediaSize()const
    {
        return glm::vec2(mCurrentImage.getWidth(), mCurrentImage.getHeight());
    }
        
    EventStatus StreamingImageSequenceMedia::startPlayback(const IEventRef&){
        if(isPlaying() && !isPaused()) return EventStatus::SUCCESS;
        play();
        flushQueue();
        ofLogNotice() << mSeqPaths[0].string() << " starting!";
        return EventStatus::SUCCESS;
    }
    
    bool StreamingImageSequenceMedia::isInit()const
    {
        return mIsInit;
    }
    
    void StreamingImageSequenceMedia::bind()
    {
        if(!mCurrentImage.isAllocated()) return;
        mCurrentImage.bind();
    }
    
    void StreamingImageSequenceMedia::unbind()
    {
        if(!mCurrentImage.isAllocated()) return;
        mCurrentImage.unbind();
    }
    
    void StreamingImageSequenceMedia::debugDraw(const ofRectangle& area, float fontsize)
    {
        if(!mCurrentImage.isAllocated()) return;
        //push
        auto style = ofGetStyle();
        auto col = style.color;
        auto fill = style.bFill;
        
        ofColor playingColor;
        if(isPlaying()){
            playingColor = ofColor(255,0,255);
        }else if(isFinished()){
            playingColor = ofColor(0,127,255);
        }else{
            playingColor = ofColor(0,255,255);
        }
        //draw
        ofSetColor(playingColor);
        ofNoFill();
        ofDrawRectangle(area);
        ofDrawLine(0, 0, area.width, area.height);
        ofDrawLine(area.width, 0, 0, area.height);
        auto font = getDebugFont(fontsize);
        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
        auto padding = 2;
        
        //current type
        ofFill();
        ofSetColor(50, 50, 50);
        auto viewName = "STREAMING IMAGE SEQUENCE MEDIA";
        
        auto sw = font->stringWidth(viewName);
        auto line = 0;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 0);
        font->drawStringAsShapes(viewName, padding, line + padding + font->getAscenderHeight() );
        
        //current file
        ofFill();
        ofSetColor(0, 0, 0);
        auto curPath = "current path: " + mSeqPaths[getState().currentFrame].string();
        
        sw = font->stringWidth(curPath);
        line = extra + (padding * 2);

        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(curPath, padding, line + padding + font->getAscenderHeight() );
        
        //current file
        ofFill();
        ofSetColor(0, 0, 0);
        std::string status = "status: ";
        
        if(isPaused()){
            status += " PAUSED";
        }else if(isPlaying()){
            status += " PLAYING";
        }else if(isFinished()){
            status += " FINISHED";
        }else{
            status += " STOPPED";
        }
        
        sw = font->stringWidth(status);
        line = (extra + (padding * 2))*2;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(playingColor);
        font->drawStringAsShapes(status, padding, line + padding + font->getAscenderHeight() );
        
        //size
        ofFill();
        ofSetColor(0, 0, 0);
        auto node = mNode.lock();
        auto nodesize = "[x: "+ofToString(node->getPosition().x)+" y: "+ofToString(node->getPosition().y)+" w: "+ofToString(area.width)+" h: "+ofToString(area.height)+"] media size [w: "+ofToString(mCurrentImage.getWidth())+" h: "+ofToString(mCurrentImage.getHeight())+"]";
        
        sw = font->stringWidth(nodesize);
        line = (extra + (padding * 2))*3;
        
        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(nodesize, padding, line + padding + font->getAscenderHeight() );
        
        //completeness
        ofFill();
        ofSetColor(0, 0, 0);
        auto position = "complete: " + ofToString(getState().getPlaybackPosition()*100) + "% frame: " + ofToString(getState().currentFrame);
        
        sw = font->stringWidth(position);
        line = (extra + (padding * 2))*4;

        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(position, padding, line + padding + font->getAscenderHeight() );
        
        ofSetColor(127, 255, 255);
        ofDrawRectangle(0, area.height-10, area.width*getState().getPlaybackPosition(), 10);
        
        //missed frames
        ofFill();
        ofSetColor(0, 0, 0);
        auto missed = "missed frames: " + ofToString(mMissedFrames) + " queue size: " + ofToString(mQueue.size());
        
        sw = font->stringWidth(missed);
        line = (extra + (padding * 2))*5;

        ofDrawRectangle(0, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255, 255);
        font->drawStringAsShapes(missed, padding, line + padding + font->getAscenderHeight() );
        
        //pop
        ofSetColor(col);
        if(fill)
            ofFill();
        else
            ofNoFill();
    }
    
    ofTexture* StreamingImageSequenceMedia::getCurrentTexture()
    {
        return &mCurrentImage;
    }
    
    void StreamingImageSequenceMedia::setLoop(const bool flag)
    {
        mState.setLoop(flag);
        flushQueue();
    }
    
    void StreamingImageSequenceMedia::setReverse(const bool flag)
    {
        mState.setReverse(flag);
        flushQueue();
    }
    
    void StreamingImageSequenceMedia::setBoomarang(const bool flag)
    {
        mState.setBoomarang(flag);
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequenceMedia::setLoopPoints( uint32_t in, uint32_t out )
    {
        mState.loopPointIn = in;
        mState.loopPointOut = out;
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequenceMedia::setLoopPointIn( uint32_t frame )
    {
        mState.loopPointOut = frame;
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequenceMedia::setLoopPointOut( uint32_t frame )
    {
        mState.loopPointOut = frame;
        if(mState.isLooping())
            flushQueue();
    }
    
    void StreamingImageSequenceMedia::setLoop( const bool flag, uint32_t inFrame, uint32_t outFrame )
    {
        mState.setLoop(flag);
        mState.loopPointIn = inFrame;
        mState.loopPointOut = outFrame;
        flushQueue();
    }
    
    void StreamingImageSequenceMedia::flushQueue()
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
    
    void StreamingImageSequenceMedia::startDataLoadingThread()
    {
        if(!mDataLoadingThreadRunning && !mDataLoadingThread){
            mDataLoadingThreadRunning = true;
            mDataLoadingThread.reset( new std::thread(&StreamingImageSequenceMedia::dataLoadingThread, this));
        }
    }
    
    void StreamingImageSequenceMedia::stopDataLoadingThread()
    {
        if(mDataLoadingThreadRunning && mDataLoadingThread){
            mDataLoadingThreadRunning = false;
            mQueue.abort();
            mDataLoadingThread->join();
            mDataLoadingThread.reset();
        }
    }
    
    void StreamingImageSequenceMedia::dataLoadingThread()
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
