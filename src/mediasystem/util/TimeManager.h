//
//  TimelineController.hpp
//  Zone2C_Pillars
//
//  Created by Mike Allison on 3/17/16.
//
//

#pragma once
#include <stdint.h>
#include <memory>
#include "mediasystem/util/BasicTimer.hpp"

namespace mediasystem {
    
    class TimeManager {
        
    public:
               
		TimeManager();
		~TimeManager() = default;
        
        //! pauses all time-dependent & timeline-dependent animation
        inline void setPaused( bool enable ){ enable ? mPlaybackState = PAUSED : mPlaybackState == UNLOCKED ? mPlaybackState = UNLOCKED : mPlaybackState = LOCKED; }
       
		//! returns whether or not the timeline is paused
		inline const bool isPaused() const { return mPlaybackState == PAUSED; }
        
        //! sets the fixed length tick in fps
        inline void setLockedFrameRate(uint32_t fps ){ mLockedFrameRate = fps; mTick = 1.0 / static_cast<double>( mLockedFrameRate ); }
		
		//! gets the fixed length tick in fps
        inline const int  getLockedFrameRate() const { return mLockedFrameRate; }

		//! sets the playback speed  - 1.0 default no change.  0.5 would be half speed and 2.0 would be double speed
		inline void setTimelineSpeed( double speed ) { mTimelineSpeed = speed; }

		//! get the playback speed  - 1.0 default no change.  0.5 would be half speed and 2.0 would be double speed
		inline const double  getTimelineSpeed() const { return mTimelineSpeed; }
        
        //! locks the animation controller to run the timeline at fixed length tick
        inline void lockFrameRate(){ mPlaybackState = LOCKED; }
		inline void lockFrameRate(uint32_t fps) { mPlaybackState = LOCKED; setLockedFrameRate(fps); }

		//! unlocks the animation controller to let the timeline run freeley
		inline void unlockFrameRate(){ mPlaybackState = UNLOCKED; }
        
        //! returns true if LOCKED
		inline const bool isFrameRateLocked() const { return mPlaybackState == LOCKED; }
        
        //! gets current animation time, similar to ci::app::getElapsedSeconds(), but it can be paused or slowed down/sped up
        inline const double getElapsedSeconds() const { return mCurrentTime; }
        
        //! returns only the number of animation frames that have occured
        inline const size_t getElapsedFrames() const { return mCurrentFrameNumber; }
        
		inline const double getLastFrameTime() { return mLastFrameTime * mTimelineSpeed; }

        //! this MUST be called every frame to update the animations based on the current state of the controller
        void tick();
        
    private:
        
        enum PlaybackState { UNLOCKED, LOCKED, PAUSED };
        
		Timer   					mFrameTimer;
        PlaybackState               mPlaybackState;
		uint32_t                    mLockedFrameRate;
		double						mTimelineSpeed;
        double                      mTick;
        double                      mCurrentTime;
		double						mLastFrameTime;
        size_t						mCurrentFrameNumber;

    };
    
}//end namespace mediasystem
