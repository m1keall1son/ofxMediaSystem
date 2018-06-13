//
//  ThreadPool.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/26/18.
//

#pragma once
#include <stdint.h>
#include <vector>
#include <thread>
#include <functional>
#include "mediasystem/util/LockingQueue.hpp"
#include "mediasystem/util/Singleton.hpp"

namespace mediasystem {
    
    using Task = std::function<void()>;
    
    class ThreadPool {
    public:
        
        ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
        ~ThreadPool();
        
        void push(const Task& t);
        void push(Task&& t);
        bool trypPush(const Task& t);
        bool tryPush(Task&& t);
        void run();
        void shutdown();
        
        inline size_t getNumThreads()const{ return mNumThreads; }

    private:
        
        void processTask();
        
        size_t mNumThreads;
        std::atomic_bool mRunning{false};
        std::mutex mTaskMutex;
        std::condition_variable mCondition;
        
        LockingQueue<Task,1024> mQueue;
        std::vector<std::unique_ptr<std::thread>> mThreads;
        
    };
    
    using ThreadPoolManager = Singleton<ThreadPool>;
    
}//end namespace mediasystem

