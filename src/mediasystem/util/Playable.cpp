//
//  Playable.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/4/18.
//

#include "Playable.h"

namespace mediasystem {
    
    void Playable::State::play()
    {
        flags &= ~PLAYSTATE_FINISHED;
        
        if (flags & PLAYSTATE_PAUSED) {
            flags &= ~PLAYSTATE_PAUSED;
        }
        else {
            if (totalFrames > 0) {
                flags &= ~PLAYSTATE_PAUSED;
                flags |= PLAYSTATE_PLAYING;
            }
        }
    }
    
    void Playable::State::stop()
    {
        flags &= ~PLAYSTATE_PLAYING;
        flags &= ~PLAYSTATE_PAUSED;
    }
    
    void Playable::State::reset()
    {
        if ((flags & PLAYSTATE_REVERSE) && !(flags & PLAYSTATE_BOOMARANG)) {
            currentFrame = totalFrames - 1;
        }
        else {
            currentFrame = 0;
        }
    }
    
    void Playable::State::pause() { flags |= PLAYSTATE_PAUSED; }
    
    Playable::State Playable::State::advance( const Playable::State& state )
    {
        Playable::State ret = state;
        
        auto loopStart = ret.loopPointIn == std::numeric_limits<uint32_t>::max() ? 0 : ret.loopPointIn;
        auto loopEnd = ret.loopPointOut == std::numeric_limits<uint32_t>::max() ? ret.totalFrames-1 : ret.loopPointOut;

        if (ret.flags & PLAYSTATE_LOOPING) {
            if (ret.flags & PLAYSTATE_REVERSE) {
                if (ret.currentFrame <= loopStart) {
                    if (state.flags & PLAYSTATE_BOOMARANG) {
                        ret.flags ^= PLAYSTATE_REVERSE;
                        ++ret.currentFrame;
                    }
                    else {
                        ret.currentFrame = loopEnd;
                    }
                }else{
                    --ret.currentFrame;
                }
            }
            else {
                if (ret.currentFrame >= loopEnd) {
                    if (ret.flags & PLAYSTATE_BOOMARANG) {
                        ret.flags ^= PLAYSTATE_REVERSE;
                        --ret.currentFrame;
                    }
                    else {
                        ret.currentFrame = loopStart;
                    }
                }else{
                    ++ret.currentFrame;
                }
            }
        }
        else {
            if (ret.flags & PLAYSTATE_REVERSE) {
                if (ret.currentFrame <= 0) {
                    ret.currentFrame = ret.totalFrames-1;
                    ret.flags |= PLAYSTATE_FINISHED;
                }else{
                    --ret.currentFrame;
                }
            }
            else {
                if (ret.currentFrame >= ret.totalFrames-1) {
                    ret.currentFrame = 0;
                    ret.flags |= PLAYSTATE_FINISHED;
                }else{
                    ++ret.currentFrame;
                }
            }
        }
        return ret;
    }
    
    Playable::Playable( float fps, const Options& options ):
        mFramerate(fps),
        mElapsedTime(0.0)
    {
        mState.flags = options.getFlags();
        auto loopPoints = options.getLoopPoints();
        mState.loopPointIn = loopPoints.first;
        mState.loopPointOut = loopPoints.second;
        mKeyFrames = std::move(options.getKeyFrames());
    }
    
    void Playable::play()
    {
        mState.play();
    }
    
    void Playable::pause()
    {
        mState.pause();
    }
    
    void Playable::stop()
    {
        mState.stop();
    }
    
    void Playable::advanceFrame()
    {
        if(mState.isFinished()){
            return;
        }
        mState = State::advance(mState);
        if(mState.isFinished()){
            stop();
        }
    }
    
    void Playable::reset()
    {
        mState.reset();
    }

    void Playable::setKeyFrame(uint32_t frame, const KeyFrameCallback& callback)
    {
        mKeyFrames.emplace(frame,callback);
    }
    
    void Playable::step( double timestep )
    {
        auto cur = timestep + mElapsedTime;
        auto rate = (1.f/mFramerate);
        if ((mState.isPlaying()) && !(mState.isPaused()) && cur >= rate) {
            mElapsedTime = 0.0;
            auto found = mKeyFrames.find(mState.currentFrame);
            if(found!=mKeyFrames.end()){
                found->second(mState);
            }
            advanceFrame();
        }else{
            mElapsedTime += timestep;
        }
    }
    
}//end namespace mediasystem
