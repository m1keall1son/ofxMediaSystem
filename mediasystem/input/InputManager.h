//
//  InputManager.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#pragma once

#include "InputComponent.h"

namespace mediasystem {
    
    class InputManager {
    public:
        
        InputManager();
        ~InputManager() = default;
        
        void connect();
        void disconnect();
        
        void update();

        void registerInputComponent(const InputComponentHandle& handle);
        void registerInputComponent(InputComponentHandle&& handle);
        
    private:
        
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
        std::map<int, std::list<InputComponentHandle>> mComponents;
        
    };
    
}//end namespace mediasystem
