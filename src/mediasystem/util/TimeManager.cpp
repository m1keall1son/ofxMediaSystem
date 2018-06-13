//
//  TimelineController.cpp
//  Zone2C_Pillars
//
//  Created by Mike Allison on 3/17/16.
//
//

#include "TimeManager.h"
#include "Log.h"

namespace mediasystem {

    TimeManager::TimeManager() :
        mLockedFrameRate(60),
        mTimelineSpeed(1.0),
        mPlaybackState(UNLOCKED),
        mCurrentFrameNumber(0),
        mCurrentTime(0.0),
        mLastFrameTime(0.0),
        mFrameTimer(true)
    {
        mTick = 1.0 / static_cast<double>(mLockedFrameRate);
    }
    
    void TimeManager::tick()
    {
        mLastFrameTime = mFrameTimer.getSeconds();
        mFrameTimer.start();
        
        switch(mPlaybackState){
            case UNLOCKED:
            {
                mCurrentTime += mLastFrameTime * mTimelineSpeed;
                mCurrentFrameNumber++;
            }
                break;
            case LOCKED:
            {
                mCurrentTime += mTick * mTimelineSpeed;
                mCurrentFrameNumber++;
            }
                break;
            case PAUSED:
            {
            }
                break;
        }
    }
    
}//end namespace mediasystem

