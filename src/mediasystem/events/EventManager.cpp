//
//  EventManager.cpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#include "EventManager.h"
#include "mediasystem/util/Log.h"

namespace mediasystem {
    
    EventManager::EventManager(int maxDequeueTime):
    mQueue([&](IEventRef& event){
        mEvents[event->getType()](event);
    }, maxDequeueTime),
    mThreadedQueue([&](IEventRef& event){
        mEvents[event->getType()](event);
    }, maxDequeueTime)
    {}
    
    void EventManager::queueEvent(const IEventRef& event)
    {
        mQueue.push(event);
    }
    
    void EventManager::queueEvent(IEventRef&& event)
    {
        mQueue.push(std::move(event));
    }
    
    void EventManager::queueThreadedEvent(const IEventRef& event)
    {
        if(!mThreadedQueue.tryPush(event)){
            MS_LOG_ERROR("Threaded queue is full or locked!");
        }
    }
    
    void EventManager::queueThreadedEvent(IEventRef&& event)
    {
        if(!mThreadedQueue.tryPush(std::move(event))){
            MS_LOG_ERROR("Threaded queue is full or locked!");
        }
    }
    
    void EventManager::triggerEvent(const IEventRef& event)
    {
        mEvents[event->getType()](event);
    }

    void EventManager::update()
    {
        mThreadedQueue.dequeue();
        mQueue.dequeue();
    }
    
    void EventManager::clear()
    {
        mThreadedQueue.reset();
        mQueue.clear();
        mEvents.clear();
    }
    
}//end namespace mediasystem
