//
//  StreamingImageSequence.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/26/18.
//

#pragma once

#include "ImageSequenceViewBase.h"
#include "mediasystem/util/LockingQueue.hpp"
#include "ofMain.h"
#include <thread>

namespace mediasystem {
    
    struct Frame {
        Frame():frame(0),img(nullptr),serial(0),finished(false){}
        Frame(uint32_t f, const std::shared_ptr<ofImage>& d, uint32_t s, bool fin):frame(f),img(d),serial(s),finished(fin){}
        std::shared_ptr<ofImage> img;
        uint32_t frame;
        uint32_t serial;
        bool finished;
        bool operator==(const Frame& rhs);
        bool operator!=(const Frame& rhs);
        bool operator<(const Frame& rhs);
        bool operator<=(const Frame& rhs);
        bool operator>(const Frame& rhs);
        bool operator>=(const Frame& rhs);
    };
    
    class StreamingImageSequenceView : public ImageSequenceViewBase, public std::enable_shared_from_this<StreamingImageSequenceView> {
    public:
        
        StreamingImageSequenceView(SceneBase* scene, float fps = 30.f, const Playable::Options& options = Playable::Options());
        StreamingImageSequenceView(SceneBase* scene, const std::filesystem::path& source, float framerate, const Playable::Options& options = Playable::Options());
        
        ~StreamingImageSequenceView();
        
        void initPaths(const std::filesystem::path& imageSequenceDir)override;
        void step(double time)override;
    
        void init()override;
        void start()override;
        void draw()override;
        void debugDraw()override;
        
        void setLoop( const bool flag = true )override;
        void setReverse(const bool flag = true)override;
        void setBoomarang(const bool flag = true)override;
        void setLoopPointIn( uint32_t frame )override;
        void setLoopPointOut( uint32_t frame )override;
        void setLoopPoints( uint32_t inFrame, uint32_t outFrame )override;
        void setLoop( const bool flag, uint32_t inFrame, uint32_t outFrame );
        
    private:
        
        void flushQueue();
        
        ofTexture* getCurrentTexture() override;
        void startDataLoadingThread();
        void stopDataLoadingThread();
        void dataLoadingThread();

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
    
}//end namespace mediasystem
