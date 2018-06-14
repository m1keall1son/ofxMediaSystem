//
//  InputManager.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#pragma once

#include "mediasystem/core/ComponentSystem.hpp"
#include "mediasystem/events/EventManager.h"
#include "mediasystem/input/InputComponent.h"

namespace mediasystem {
    
    class InputComponent;
    using InputComponentHandle = std::weak_ptr<InputComponent>;

    class InputSystem {
    public:
        
        InputSystem(EventManager& eventManager);
        ~InputSystem() = default;
        
        void connect();
        void disconnect();

        void update();

    private:
        
        void onInitEvent(const IEventRef& event);
        void onUpdateEvent(const IEventRef& event);
        void onNewInputComponent(const IEventRef& event);

        enum EventType { MOUSE_MOVE, MOUSE_EXIT, MOUSE_PRESSED, MOUSE_RELEASED, MOUSE_DRAGGED, MOUSE_SCROLL, KEY_PRESSED, KEY_RELEASED };
        
        void mouseMove( ofMouseEventArgs& mouse );
        void mouseExit( ofMouseEventArgs& mouse );
        void mousePressed( ofMouseEventArgs& mouse );
        void mouseReleased( ofMouseEventArgs& mouse );
        void mouseDragged( ofMouseEventArgs& mouse );
        void mouseScrollWheel( ofMouseEventArgs& mouse );
        void keyPressed( ofKeyEventArgs& key );
        void keyReleased( ofKeyEventArgs& key );
        
        bool mConnected{false};
        std::deque<std::pair<EventType, ofKeyEventArgs>> mKeyEvents;
        std::deque<std::pair<EventType, ofMouseEventArgs>> mMouseEvents;
        std::unordered_map<int, std::list<InputComponentHandle>> mComponentsByZIndex;
        EventManager& mEventManager;
        
    };
    
}//end namespace mediasystem
