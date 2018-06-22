//
//  GlobalEvents.h
//  WallTest
//
//  Created by Michael Allison on 6/19/18.
//

#pragma once

#include "mediasystem/events/EventManager.h"
#include "mediasystem/util/Singleton.hpp"

namespace mediasystem {
    
    struct SystemInit : Event<SystemInit> {};
    struct SystemPostInit : Event<SystemPostInit> {};
    struct SystemShutdown : Event<SystemShutdown> {};
    struct SystemReset : Event<SystemReset> {};
    
    struct ofGlobalEventManager : public EventManager {
        ofGlobalEventManager();
        ~ofGlobalEventManager();
    };
    
    using GlobalEventManager = Singleton<ofGlobalEventManager>;
        
    void queueGlobalEvent(const IEventRef& event);
    void queueGlobalEvent(IEventRef&& event);
    void triggerGlobalEvent(const IEventRef& event);
    void queueThreadedGlobalEvent(const IEventRef& event);
    void queueThreadedGlobalEvent(IEventRef&& event);
    
    template<typename EventType, typename...Args>
    void queueGlobalEvent(Args&&...args){
        static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
        queueGlobalEvent(std::make_shared<EventType>(std::forward<Args>(args)...));
    }
    
    template<typename EventType, typename...Args>
    void queueThreadedGlobalEvent(Args&&...args){
        static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
        queueThreadedGlobalEvent(std::make_shared<EventType>(std::forward<Args>(args)...));
    }
    
    template<typename EventType, typename...Args>
    void triggerGlobalEvent(Args&&...args){
        static_assert( std::is_base_of<IEvent, EventType>::value, "EventType must derive from IEvent.");
        triggerGlobalEvent(std::make_shared<EventType>(std::forward<Args>(args)...));
    }
    
    template<typename EventType>
    void addGlobalEventDelegate(EventDelegate delegate){
        auto& g_em = GlobalEventManager::get();
        g_em.addDelegate<EventType>(delegate);
    }
    
    template<typename EventType>
    void removeGlobalEventDelegate(EventDelegate delegate){
        auto& g_em = GlobalEventManager::get();
        g_em.removeDelegate<EventType>(delegate);
    }
    
}//end namespace mediasystem
