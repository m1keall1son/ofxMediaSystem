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
#include "mediasystem/util/TimedQueue.hpp"
#include "mediasystem/util/TimedLockingQueue.hpp"
#include "MultiCastDelegate.h"
#include "Delegate.h"
#include "TypeId.hpp"

namespace mediasystem {
    
    using EventDelegate = SA::delegate<void(const IEventRef&)>;
    using EventMulticaster = SA::multicast_delegate<void(const IEventRef&)>;
        
    class EventManager {
    public:
        
        EventManager(int mexDequeueTime = TimedQueue<IEventRef>::NO_TIME_LIMIT);
        ~EventManager() = default;
        
        void update();
    
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
            auto event = std::make_shared<EventType>(std::forward<Args>(args)...);
            mEvents[type_id<EventType>](event);
        }
        void triggerEvent(const IEventRef& event);
        
        template<typename EventType>
        void addDelegate(EventDelegate delegate){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            mEvents[type_id<EventType>] += delegate;
        }
        
        template<typename EventType>
        void removeDelegate(EventDelegate delegate){
            static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
            mEvents[type_id<EventType>] -= delegate;
        }
        
        void clear();

    private:
                
        TimedQueue<IEventRef> mQueue;
        TimedLockingQueue<IEventRef,1024> mThreadedQueue;
        std::map<type_id_t, EventMulticaster> mEvents;
        
    };
}//end namespace mediasystem
