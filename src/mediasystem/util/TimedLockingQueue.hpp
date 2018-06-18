//
//  MessageQueue.hpp
//  lpNet
//
//  Created by Michael Allison on 5/26/18.
//

#pragma once

#include "LockingQueue.hpp"

namespace mediasystem {

    template<typename T, size_t MAX_SIZE>
    class TimedLockingQueue : public LockingQueue<T, MAX_SIZE>{
    public:
        
        static const int NO_TIME_LIMIT = -1;

        using Ref = std::shared_ptr<TimedLockingQueue>;
        using DequeueHandler = std::function<bool(T&)>;
        
        explicit TimedLockingQueue(DequeueHandler handler, int maxDequeueTime = NO_TIME_LIMIT):
            mMaxDequeueTime(maxDequeueTime),
            mHandler(handler)
        {}
        
        virtual ~TimedLockingQueue() = default;
        
        inline void dequeue(){
            T val;
            auto start = std::chrono::high_resolution_clock::now();
            int elapsed = 0;
            int count = 0;
            if(mMaxDequeueTime < 0){
                while(true){
                    auto success = LockingQueue<T, MAX_SIZE>::tryPop(val);
                    if(success){
                        ++count;
                        bool ret = true;
                        if(mHandler){
                            ret = mHandler(val);
                        }
                        if(!ret)
                            break;
                    }else{
                        if(count > 0){
                            elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                        }
                        break;
                    }
                    elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                }
            }else{
                while(true){
                    if(elapsed >= mMaxDequeueTime){
                        break;
                    }
                    auto success = LockingQueue<T, MAX_SIZE>::tryPop(val);
                    if(success){
                        ++count;
                        bool ret = true;
                        if(mHandler){
                            ret = mHandler(val);
                        }
                        if(!ret)
                            break;
                    }else{
                        if(count > 0){
                            elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                        }
                        break;
                    }
                    elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                }
            }
        }
        
        inline void setMaxDequeueTime( int maxMs ){ mMaxDequeueTime = maxMs; }
        inline int getMaxDequeueTime() const { return mMaxDequeueTime; }

    protected:
        
        int mMaxDequeueTime{NO_TIME_LIMIT};
        DequeueHandler mHandler;
    };

}//end namespace media system
