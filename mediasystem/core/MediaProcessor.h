//
//  MediaProcessor.h
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include <stdio.h>
#include <chrono>
#include "mediasystem/util/BasicTimer.hpp"
#include "mediasystem/util/EstimatedMovingAverage.hpp"

namespace mediasystem {
    
    class MediaProcessor {
    public:
        
        enum class Status { SUCCESS, FAIL, ABORT, SKIP };
        
        MediaProcessor() = default;
        virtual ~MediaProcessor() = default;
        
        MediaProcessor(const MediaProcessor&) = delete;
        MediaProcessor& operator=(const MediaProcessor&) = delete;
        
        Status process();
        
        virtual void enable();
        virtual void disable();
        
        virtual const char* getName() const = 0;
        virtual void init() = 0;
        virtual void shutdown() = 0;

        inline const bool isEnabled() const { return mEnabled; }
        inline const double getLastProcessDuration() const { return mProcessTime; }
        inline const double getAvgProcessDuration() const { return mAvg.getAverage(); }
        
    protected:
        
        virtual void doProcess() = 0;
        
    private:
        mediasystem::Timer mTimer;
        double mProcessTime{0.0};
        ExponentialMovingAverage<double> mAvg{0.9, 1.0};
        bool mEnabled{true};
    };
    
}//end namespace mediasystem


