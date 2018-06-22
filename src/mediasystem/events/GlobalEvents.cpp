//
//  GlobalEvents.cpp
//  WallTest
//
//  Created by Michael Allison on 6/21/18.
//

#include "GlobalEvents.h"
#include "ofMain.h"

namespace mediasystem {
    
    static void processGlobalEvents(ofEventArgs& args)
    {
        auto& g_em = GlobalEventManager::get();
        g_em.processEvents();
    }
    
    ofGlobalEventManager::ofGlobalEventManager(){
        ofAddListener(ofEvents().update, &processGlobalEvents);
    }
    
    ofGlobalEventManager::~ofGlobalEventManager(){
        ofRemoveListener(ofEvents().update, &processGlobalEvents);
    }
    
    void queueGlobalEvent(const IEventRef& event)
    {
        auto& g_em = GlobalEventManager::get();
        g_em.queueEvent(event);
    }
    
    void queueGlobalEvent(IEventRef&& event)
    {
        auto& g_em = GlobalEventManager::get();
        g_em.queueEvent(std::move(event));
    }
    
    void triggerGlobalEvent(const IEventRef& event)
    {
        auto& g_em = GlobalEventManager::get();
        g_em.triggerEvent(event);
    }
    
    void queueThreadedGlobalEvent(const IEventRef& event)
    {
        auto& g_em = GlobalEventManager::get();
        g_em.queueThreadedEvent(event);
    }
    
    void queueThreadedGlobalEvent(IEventRef&& event)
    {
        auto& g_em = GlobalEventManager::get();
        g_em.queueThreadedEvent(std::move(event));
    }
    
}//end namespace mediasystem
