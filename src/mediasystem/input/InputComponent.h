//
//  InputComponent.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#pragma once
#include <memory>
#include "mediasystem/events/IEvent.h"
#include "ofMain.h"

namespace mediasystem {
    
    class Entity;
    
    struct DragState {
        glm::vec2 mouseStart;
        glm::vec2 positionStart;
        glm::vec2 delta;
    };
    
    using MouseInputHandler = std::function<void(Entity&, const ofMouseEventArgs&)>;
    using MouseDragHandler = std::function<void(Entity&, const ofMouseEventArgs&, const DragState&)>;
    using KeyboardInputHandler = std::function<void(Entity&, const ofKeyEventArgs&)>;
    using TouchInputHandler = std::function<void(Entity&, const ofTouchEventArgs&)>;

    class InputHandlers {
    public:
        
        InputHandlers() = default;
        ~InputHandlers() = default;
        
        inline InputHandlers& mouseInHandler(MouseInputHandler handler){ mOnMouseIn = std::move(handler); return *this; }
        inline InputHandlers& mouseOutHandler(MouseInputHandler handler){ mOnMouseOut = std::move(handler); return *this; }
        inline InputHandlers& mouseHoverHandler(MouseInputHandler handler){ mOnMouseHover = std::move(handler); return *this; }
        inline InputHandlers& mousePressedHandler(MouseInputHandler handler){ mOnMousePressed = std::move(handler); return *this; }
        inline InputHandlers& mouseReleasedHandler(MouseInputHandler handler){ mOnMouseReleased = std::move(handler); return *this; }
        inline InputHandlers& mouseDraggedHandler(MouseDragHandler handler){ mOnMouseDragged = std::move(handler); return *this; }
        inline InputHandlers& mouseScrollWheelHandler(MouseInputHandler handler){ mOnMouseScrollWheel = std::move(handler); return *this; }
        inline InputHandlers& keyPressedHandler(KeyboardInputHandler handler){ mOnKeyPressed = std::move(handler); return *this; }
        inline InputHandlers& keyReleasedHandler(KeyboardInputHandler handler){ mOnKeyReleased = std::move(handler); return *this; }
        
    private:
        MouseInputHandler mOnMouseIn{nullptr};
        MouseInputHandler mOnMouseOut{nullptr};
        MouseInputHandler mOnMouseHover{nullptr};
        MouseInputHandler mOnMousePressed{nullptr};
        MouseInputHandler mOnMouseReleased{nullptr};
        MouseDragHandler mOnMouseDragged{nullptr};
        MouseInputHandler mOnMouseScrollWheel{nullptr};
        KeyboardInputHandler mOnKeyPressed{nullptr};
        KeyboardInputHandler mOnKeyReleased{nullptr};
        friend class InputComponent;
    };
    
    class InputComponent {
    public:
        
        InputComponent( Entity& context, const ofRectangle& screenBounds, int z_index = 0, InputHandlers handlers = InputHandlers() );
        ~InputComponent() = default;
        
        void update();
        
        bool check(const glm::vec2& point)const;
        bool isEnabled()const;
        void setEnabled(bool enable);
        
        inline int getZIndex()const{ return mZIndex; }
        inline bool isPressed()const{ return mPressed; }
        inline bool isHovering()const{ return mHovering; }
        inline const ofRectangle& getScreenArea()const{ return mScreenBounds; }
        inline void setScreenArea(const ofRectangle& screenArea){ mScreenBounds = screenArea; }

        bool mouseMove(const ofMouseEventArgs& mouse);
        bool mouseExit(const ofMouseEventArgs& mouse);
        bool mouseDragged(const ofMouseEventArgs& mouse);
        bool mousePressed(const ofMouseEventArgs& mouse);
        bool mouseReleased(const ofMouseEventArgs& mouse);
        bool mouseScrollWheel(const ofMouseEventArgs& mouse);
        
        void keyPressed(const ofKeyEventArgs& key);
        void keyReleased(const ofKeyEventArgs& key);
        
        inline void setOnMouseInHandler(MouseInputHandler handler ){ mHandlers.mOnMouseIn = std::move(handler); }
        inline void setOnMouseOutHandler(MouseInputHandler handler ){ mHandlers.mOnMouseOut = std::move(handler); }
        inline void setOnMouseHoverHandler(MouseInputHandler handler ){ mHandlers.mOnMouseHover = std::move(handler); }
        inline void setOnMousePressedHandler(MouseInputHandler handler ){ mHandlers.mOnMousePressed = std::move(handler); }
        inline void setOnMouseReleasedHandler(MouseInputHandler handler ){ mHandlers.mOnMouseReleased = std::move(handler); }
        inline void setOnMouseDraggedHandler(MouseDragHandler handler ){ mHandlers.mOnMouseDragged = std::move(handler); }
        inline void setOnMouseScrollWheelHandler(MouseInputHandler handler ){ mHandlers.mOnMouseScrollWheel = std::move(handler); }
        inline void setOnKeyPressedHandler(KeyboardInputHandler handler ){ mHandlers.mOnKeyPressed = std::move(handler); }
        inline void setOnKeyReleasedHandler(KeyboardInputHandler handler ){ mHandlers.mOnKeyReleased = std::move(handler); }
        
    private:
        
        Entity& mContext;
        std::weak_ptr<ofNode> mNode;
        bool mHovering{false};
        bool mPressed{false};
        int mZIndex{0};
        bool mEnabled{true};
        ofRectangle mScreenBounds;
        glm::vec2 mSize;
        DragState mDragState;
        InputHandlers mHandlers;
    };
    
}//end namespace mediasystem

