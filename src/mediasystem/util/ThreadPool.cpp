//
//  ThreadPool.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/26/18.
//

#include "ThreadPool.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/Profiler.hpp"

namespace mediasystem {
    
    ThreadPool::ThreadPool(size_t num_threads):mNumThreads(num_threads)
    {
        mThreads.resize(num_threads);
    }
    
    ThreadPool::~ThreadPool()
    {
        shutdown();
    }
    
    void ThreadPool::push(const Task& t)
    {
        mQueue.push(t);
    }
    
    void ThreadPool::push(Task&& t)
    {
        mQueue.push(std::move(t));
    }
    
    bool ThreadPool::trypPush(const Task& t)
    {
        return mQueue.tryPush(t);
    }
    
    bool ThreadPool::tryPush(Task&& t)
    {
        return mQueue.tryPush(std::move(t));
    }
    
    void ThreadPool::processTask()
    {
        while(mRunning){
            Task t = mQueue.pop();
            if(t){
                try{
                    t();
                }catch(const std::exception& e){
                    MS_LOG_ERROR("processTask exception: " << e.what());
                    
                }catch(...){
                    MS_LOG_ERROR("processTask unknown error");
                }
            }
        }
    }
    
    void ThreadPool::run()
    {
        if(mRunning)
            return;
        
        mRunning = true;
        for(auto & thread:mThreads){
            thread.reset( new std::thread( &ThreadPool::processTask, this ) );
        }
    }
    
    void ThreadPool::shutdown()
    {
        if(mRunning){
            mRunning = false;
            mQueue.abort();
            for(auto& thread : mThreads){
                if(thread->joinable()){
                    thread->join();
                }else{
                    MS_LOG_ERROR("ThreadPool thread not joinable!!");
                }
            }
        }
    }
    
}//end namespace mediasystem
