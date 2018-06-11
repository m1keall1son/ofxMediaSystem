//
//  InputComponent.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#pragma once
#include <memory>
#include "ofMain.h"

namespace mediasystem {
    
    class ViewBase;
    using InputComponentHandle = std::weak_ptr<class InputComponent>;
    
    struct DragState {
        glm::vec2 mouseStart;
        glm::vec2 viewPositionStart;
        glm::vec2 delta;
    };
    
    using MouseInputHandler = std::function<void(ViewBase*, const ofMouseEventArgs&)>;
    using MouseDragHandler = std::function<void(ViewBase*, const ofMouseEventArgs&, const DragState&)>;
    using KeyboardInputHandler = std::function<void(ViewBase*, const ofKeyEventArgs&)>;
    using TouchInputHandler = std::function<void(ViewBase*, const ofTouchEventArgs&)>;

    class InputComponent : public std::enable_shared_from_this<InputComponent> {
    public:
        
        InputComponent( ViewBase* context, const ofRectangle& screenBounds, int z_index = 0 );
        ~InputComponent() = default;
        
        void update();
        
        bool check(const glm::vec2& point)const;
        bool isEnabled()const;
        
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
        
        inline void setOnMouseInHandler( const MouseInputHandler& handler ){ mOnMouseIn = handler; }
        inline void setOnMouseOutHandler( const MouseInputHandler& handler ){ mOnMouseOut = handler; }
        inline void setOnMouseHoverHandler( const MouseInputHandler& handler ){ mOnMouseHover = handler; }
        inline void setOnMousePressedHandler( const MouseInputHandler& handler ){ mOnMousePressed = handler; }
        inline void setOnMouseReleasedHandler( const MouseInputHandler& handler ){ mOnMouseReleased = handler; }
        inline void setOnMouseDraggedHandler( const MouseDragHandler& handler ){ mOnMouseDragged = handler; }
        inline void setOnMouseScrollWheelHandler( const MouseInputHandler& handler ){ mOnMouseScrollWheel = handler; }
        inline void setOnKeyPressedHandler( const KeyboardInputHandler& handler ){ mOnKeyPressed = handler; }
        inline void setOnKeyReleasedHandler( const KeyboardInputHandler& handler ){ mOnKeyReleased = handler; }

        //TODO touch events

        InputComponentHandle getHandle();
        
    private:
        
        ViewBase* mContext{nullptr};
        bool mHovering{false};
        bool mPressed{false};
        int mZIndex{0};
        ofRectangle mScreenBounds;
        glm::vec2 mSize;
        DragState mDragState;
        MouseInputHandler mOnMouseIn{nullptr};
        MouseInputHandler mOnMouseOut{nullptr};
        MouseInputHandler mOnMouseHover{nullptr};
        MouseInputHandler mOnMousePressed{nullptr};
        MouseInputHandler mOnMouseReleased{nullptr};
        MouseDragHandler mOnMouseDragged{nullptr};
        MouseInputHandler mOnMouseScrollWheel{nullptr};
        KeyboardInputHandler mOnKeyPressed{nullptr};
        KeyboardInputHandler mOnKeyReleased{nullptr};
        
    };
    
}//end namespace mediasystem

