//
//  LockingQueue.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/26/18.
//

#pragma once
#include <deque>
#include <atomic>
#include <mutex>
#include <memory>
#include <algorithm>
#include <condition_variable>

namespace mediasystem {
    
    template<typename T, size_t max>
    class LockingQueue {
    public:
        
        const size_t MAX_SIZE = max;
        
        LockingQueue() = default;
        virtual ~LockingQueue() = default;
        
        bool full()const
        {
            bool f = false;
            {
                std::lock_guard<std::mutex> lock(mMutex);
                f =  mQueue.size() == MAX_SIZE;
            }
            return f;
        }
        
        size_t numAvailable() const
        {
            size_t available = 0;
            {
                std::lock_guard<std::mutex> lk(mMutex);
                available = MAX_SIZE - mQueue.size();
            }
            return available;
        }
        
        bool empty()const
        {
            bool e = true;
            {
                std::lock_guard<std::mutex> lock(mMutex);
                e = mQueue.size() == 0;
            }
            return e;
        }
        
        size_t size() const
        {
            size_t s = 0;
            {
                std::lock_guard<std::mutex> lk(mMutex);
                s = mQueue.size();
            }
            return s;
        }
        
        T pop()
        {
            T ret;
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mConditionEmpty.wait(lock, [&](){ return mQueue.size() > 0 || mAbort; });
                if(mAbort || mQueue.empty()){
                    lock.unlock();
                    mConditionFull.notify_one();
                    return ret;
                }
                ret = mQueue.front();
                mQueue.pop_front();
            }
            mConditionFull.notify_one();
            return ret;
        }
        
        bool pop(T& ret, uint32_t ms)
        {
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mConditionEmpty.wait_for(lock, std::chrono::milliseconds(ms), [&](){ return mQueue.size() > 0 || mAbort; });
                if(mAbort || mQueue.empty()){
                    lock.unlock();
                    mConditionFull.notify_one();
                    return false;
                }
                ret = std::move(mQueue.front());
                mQueue.pop_front();
            }
            mConditionFull.notify_one();
            return true;
        }
        
        bool tryPop(T& val)
        {
            if(mMutex.try_lock()){
                auto ret = false;
                if(!mQueue.empty()){
                    val = std::move(mQueue.front());
                    mQueue.pop_front();
                    ret = true;
                }
                mMutex.unlock();
                mConditionFull.notify_one();
                return ret;
            }else{
                //lock busy
                return false;
            }
        }
        
        void push(const T& val)
        {
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mConditionFull.wait(lock,[&](){
                    return (mQueue.size() != MAX_SIZE) || mAbort;
                });
                if(mAbort){
                    lock.unlock();
                    mConditionEmpty.notify_one();
                    return;
                }
                mQueue.push_back(val);
            }
            mConditionEmpty.notify_one();
        }
        
        void push(T&& val)
        {
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mConditionFull.wait(lock,[&](){
                    return (mQueue.size() != MAX_SIZE) || mAbort;
                });
                if(mAbort){
                    lock.unlock();
                    mConditionEmpty.notify_one();
                    return;
                }
                mQueue.emplace_back(std::move(val));
            }
            mConditionEmpty.notify_one();
        }
        
        bool push(const T& val, uint32_t ms)
        {
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mConditionFull.wait_for(lock, std::chrono::milliseconds(ms), [&](){
                    return (mQueue.size() != MAX_SIZE) || mAbort;
                });
                if(mAbort || mQueue.size() == MAX_SIZE){
                    lock.unlock();
                    mConditionEmpty.notify_one();
                    return;
                }
                mQueue.push_back(val);
            }
            mConditionEmpty.notify_one();
        }
        
        bool push(T&& val, uint32_t ms)
        {
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mConditionFull.wait_for(lock, std::chrono::milliseconds(ms), [&](){
                    return (mQueue.size() != MAX_SIZE) || mAbort;
                });
                if(mAbort || mQueue.size() == MAX_SIZE){
                    lock.unlock();
                    mConditionEmpty.notify_one();
                    return;
                }
                mQueue.emplace_back(val);
            }
            mConditionEmpty.notify_one();
        }
        
        bool tryPush(const T& val)
        {
            if(mMutex.try_lock()){
                auto ret = false;
                if(mQueue.size() != MAX_SIZE){
                    mQueue.push_back(val);
                    ret = true;
                }
                mMutex.unlock();
                mConditionEmpty.notify_one();
                return ret;
            }else{
                //lock busy
                return false;
            }
        }
        
        bool tryPush(T&& val)
        {
            if(mMutex.try_lock()){
                auto ret = false;
                if(mQueue.size() != MAX_SIZE){
                    mQueue.emplace_back(std::move(val));
                    ret = true;
                }
                mMutex.unlock();
                mConditionEmpty.notify_one();
                return ret;
            }else{
                //lock busy
                return false;
            }
        }
        
        T* peekBack()
        {
            if(!empty()){
                return &mQueue.back();
            }
            else return nullptr;
        }
        
        T* peekFront()
        {
            if(!empty()){
                return &mQueue.front();
            }
            else return nullptr;
        }
        
        T* peek(size_t index)
        {
            if(!empty() && index < size()){
                return &mQueue[index];
            }
            else return nullptr;
        }
        
        void signal()
        {
            mConditionFull.notify_one();
            mConditionEmpty.notify_one();
        }
        
        void abort()
        {
            mAbort = true;
            signal();
        }
        
        void reset()
        {
            mAbort = false;
            std::lock_guard<std::mutex> lock(mMutex);
            mQueue.clear();
        }
        
        void sort( const std::function<bool(const T&,const T&)>& predicate){
            if(predicate){
                std::lock_guard<std::mutex> lock(mMutex);
                std::sort(mQueue.begin(), mQueue.end(), predicate);
            }
        }
        
        void flush(size_t keep = 0){
            {
                std::lock_guard<std::mutex> lock(mMutex);
                size_t count = mQueue.size() - keep;
                if(count == 0)
                    return;
                for(size_t i = 0; i < count; i++){
                    mQueue.pop_back();
                }
            }
            mConditionFull.notify_one();
        }
        
    private:
        
        std::atomic_bool mAbort{false};
        mutable std::mutex mMutex;
        std::condition_variable mConditionFull;
        std::condition_variable mConditionEmpty;
        std::deque<T> mQueue;
    };
    
}//end namespace mediasystem


