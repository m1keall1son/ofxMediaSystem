//
//  MessageQueue.hpp
//  lpNet
//
//  Created by Michael Allison on 5/26/18.
//

#pragma once

#include <deque>

namespace mediasystem {

    template<typename T>
    class TimedQueue {
    public:
        
        static const int NO_TIME_LIMIT = -1;

        using Ref = std::shared_ptr<TimedQueue>;
        using DequeueHandler = std::function<bool(T&)>;
        
        explicit TimedQueue(DequeueHandler handler, int maxDequeueTime = NO_TIME_LIMIT):
            mMaxDequeueTime(maxDequeueTime),
            mHandler(handler)
        {}
        
        virtual ~TimedQueue() = default;
        
        inline void dequeue(){
            T val;
            auto start = std::chrono::high_resolution_clock::now();
            int elapsed = 0;
            int count = 0;
            if(mMaxDequeueTime < 0){
                while(true){
                    auto success = pop(val);
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
                    auto success = pop(val);
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
        
        inline void push(T&& val)
        {
            mQueue.emplace_back(std::move(val));
        }
        
        inline void push(const T& val)
        {
            mQueue.push_back(val);
        }
        
        void clear(){
            mQueue.clear();
        }

    protected:
        
        inline bool pop(T& val)
        {
            if(mQueue.empty())
                return false;
            val = std::move(mQueue.front());
            mQueue.pop_front();
            return true;
        }
        
        int mMaxDequeueTime{NO_TIME_LIMIT};
        DequeueHandler mHandler;
        std::deque<T> mQueue;
        
    };

}//end namespace media system
