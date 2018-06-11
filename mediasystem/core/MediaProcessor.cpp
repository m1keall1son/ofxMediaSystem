//
//  MediaProcessor.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "MediaProcessor.h"
#include "mediasystem/util/Log.h"

namespace mediasystem {
    
    MediaProcessor::Status MediaProcessor::process()
    {
        if(!mEnabled) return MediaProcessor::Status::SKIP;
        
        auto ret = MediaProcessor::Status::SUCCESS;
        mTimer.start();
        try{
            doProcess();
        }catch(const std::exception& e){
            MS_LOG_ERROR(getName() << "process failed: " << e.what());
            ret = MediaProcessor::Status::FAIL;
        }
        mProcessTime = mTimer.getSeconds();
        mAvg.filter(mProcessTime);
        
        return ret;
    }
    
    void MediaProcessor::enable()
    {
        mEnabled = true;
    }
    
    void MediaProcessor::disable()
    {
        mEnabled = false;
        mTimer.stop();
    }
    
}//end namespace mediasystem

