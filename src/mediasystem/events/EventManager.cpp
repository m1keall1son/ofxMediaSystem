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
        auto& delegateList = mEvents[event->getType()];
        if(!delegateList.empty()){
            auto ret = multicast(delegateList, event);
            switch (ret){
                case EventStatus::DEFER_EVENT:
                {
                    deferEvent(event);
                }break;
                case EventStatus::ABORT_ALL_QUEUED_EVENTS_OF_THIS_TYPE:
                {
                    //todo abort this type!
                }break;
                default: break;
            }
        }
        return true;
    }, maxDequeueTime),
    mThreadedQueue([&](IEventRef& event){
        auto& delegateList = mEvents[event->getType()];
        if(!delegateList.empty()){
            auto ret = multicast(delegateList, event);
            switch (ret){
                case EventStatus::DEFER_EVENT:
                {
                    deferEvent(event);
                }break;
                case EventStatus::ABORT_ALL_QUEUED_EVENTS_OF_THIS_TYPE:
                {
                    //todo abort this type!
                }break;
                default: break;
            }
        }
        return true;
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
    
    void EventManager::deferEvent(const IEventRef& event)
    {
        auto found = std::find_if( mDeferedEvents.begin(), mDeferedEvents.end(), [&event](const IEventRef& e){
            return event->getType() == e->getType();
        });
        if(found == mDeferedEvents.end()){
            mDeferedEvents.push_back(event);
        }else{
            //todo this doesn't preserve any ordering...if that matters...
            *found = event;
        }
    }
    
    void EventManager::triggerEvent(const IEventRef& event)
    {
        auto& delegateList = mEvents[event->getType()];
        if(!delegateList.empty()){
            auto ret = multicast(delegateList, event);
            switch (ret){
                case EventStatus::DEFER_EVENT:
                {
                    deferEvent(event);
                }break;
                case EventStatus::ABORT_ALL_QUEUED_EVENTS_OF_THIS_TYPE:
                {
                   //todo abort this type!
                }break;
                default: break;
            }
        }
    }
    
    EventStatus EventManager::multicast(EventDelegateList& list, const IEventRef& event)
    {
        auto it = list.begin();
        auto end = list.end();
        while(it != end){
            auto ret = (*it)(event);
            switch(ret){
                case EventStatus::FAILED:{
                    MS_LOG_ERROR("Event delegate failed during processing of event: " /* todo overload stream operator */);
                }break;
                case EventStatus::ABORT_THIS_EVENT:{
                    MS_LOG_WARNING("Aborting from event multicast!");
                    return EventStatus::ABORT_THIS_EVENT;
                }break;
                case EventStatus::DEFER_EVENT:{
                    MS_LOG_VERBOSE("queueing this event for the next pass, aborting multicast.");
                    return EventStatus::DEFER_EVENT;
                }break;
                case EventStatus::ABORT_ALL_QUEUED_EVENTS_OF_THIS_TYPE:{
                    MS_LOG_VERBOSE("Aborting all remaining events of type: " /* todo overload stream operator */);
                    return EventStatus::ABORT_ALL_QUEUED_EVENTS_OF_THIS_TYPE;
                }break;
                case EventStatus::REMOVE_THIS_DELEGATE:{
                    it = list.erase(it);
                }break;
                default: ++it; break;
            }
        }
        return EventStatus::SUCCESS;
    }

    void EventManager::processEvents()
    {
        mThreadedQueue.dequeue();
        mQueue.dequeue();
        if(!mDeferedEvents.empty()){
            for(auto & defered : mDeferedEvents){
                mQueue.push(defered);
            }
            mDeferedEvents.clear();
        }
    }
    
    void EventManager::clearQueues()
    {
        mThreadedQueue.reset();
        mQueue.clear();
        mDeferedEvents.clear();
    }
    
    void EventManager::clearDelegates()
    {
        for(auto & delegateList : mEvents){
            delegateList.second.clear();
        }
        mEvents.clear();
    }
    
}//end namespace mediasystem
