//
//  EventManager.hpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#pragma once

#include <map>
#include "ofMain.h"
#include "IEvent.h"
#include "mediasystem/util/Singleton.hpp"
#include "mediasystem/util/Log.h"
#include "mediasystem/util/TimedQueue.hpp"
#include "mediasystem/util/TimedLockingQueue.hpp"
#include "MultiCastDelegate.h"
#include "Delegate.h"
#include "mediasystem/core/TypeID.hpp"

namespace mediasystem {
    
    enum class EventStatus {
        SUCCESS,
        FAILED,
        ABORT_THIS_EVENT,
        ABORT_ALL_QUEUED_EVENTS_OF_THIS_TYPE,
        REMOVE_THIS_DELEGATE,
        DEFER_EVENT
    };
    
    using EventDelegate = SA::delegate<EventStatus(const IEventRef&)>;
    using EventDelegateList = std::list<EventDelegate>;
        
    class EventManager {
    public:
        
        EventManager(int mexDequeueTime = TimedQueue<IEventRef>::NO_TIME_LIMIT);
        virtual ~EventManager() = default;
        
        void processEvents();
    
        template<typename EventType, typename...Args>
        void queueEvent(Args&&...args){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            queueEvent(std::make_shared<EventType>(std::forward<Args>(args)...));
        }
        void queueEvent(const IEventRef& event);
        void queueEvent(IEventRef&& event);
        
        template<typename EventType, typename...Args>
        void queueThreadedEvent(Args&&...args){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            queueThreadedEvent(std::make_shared<EventType>(std::forward<Args>(args)...));
        }
        void queueThreadedEvent(const IEventRef& event);
        void queueThreadedEvent(IEventRef&& event);
        
        template<typename EventType, typename...Args>
        void triggerEvent(Args&&...args){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            triggerEvent(std::make_shared<EventType>(std::forward<Args>(args)...));
        }
        void triggerEvent(const IEventRef& event);
        
        template<typename EventType>
        void addDelegate(EventDelegate delegate){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            auto& list = mEvents[type_id<EventType>];
            list.emplace_back(std::move(delegate));
        }
        
        template<typename EventType>
        void removeDelegate(EventDelegate delegate){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            auto& list = mEvents[type_id<EventType>];
            auto found = std::find(list.begin(), list.end(), delegate);
            if(found != list.end()){
                list.erase(found);
            }else{
                MS_LOG_ERROR("Attemping to remove an unknown delegate");
            }
        }
        
        void clearQueues();
        void clearDelegates();

    private:
        
        static EventStatus multicast(EventDelegateList& list, const IEventRef& event);
        
        void deferEvent(const IEventRef& event);
        
        TimedQueue<IEventRef> mQueue;
        TimedLockingQueue<IEventRef,1024> mThreadedQueue;
        std::vector<IEventRef> mDeferedEvents;
        std::map<type_id_t, EventDelegateList> mEvents;
    };
    
    using GlobalEventManager = Singleton<EventManager>;
        
}//end namespace mediasystem
