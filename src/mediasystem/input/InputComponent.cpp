//
//  InputComponent.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#include "InputComponent.h"
#include "mediasystem/core/Entity.h"

namespace mediasystem {
        
    InputComponent::InputComponent( Entity& context, const ofRectangle& screenBounds, int z_index, InputHandlers handlers ) :
        mContext(context),
        mZIndex(z_index),
        mScreenBounds(screenBounds),
        mSize(screenBounds.width, screenBounds.height),
        mHandlers(std::move(handlers))
    {
        mNode = context.getComponent<ofNode>();
    }
    
    void InputComponent::update()
    {
        if(auto node = mNode.lock()){
            auto pos = node->getGlobalPosition();
            auto scale = node->getGlobalScale();
            mScreenBounds = ofRectangle( pos.x, pos.y, mSize.x * scale.x, mSize.y * scale.y );
        }
    }
    
    void InputComponent::setEnabled(bool enable)
    {
        mEnabled = enable;
    }
    
    bool InputComponent::isEnabled()const
    {
        return mEnabled;
    }
    
    bool InputComponent::check(const glm::vec2& point)const
    {
        return mScreenBounds.inside(point);
    }
    
    bool InputComponent::mouseMove(const ofMouseEventArgs& mouse)
    {
    
        if(check(mouse)){
            
            if(mHovering){
                if(mHandlers.mOnMouseHover && mEnabled)
                    mHandlers.mOnMouseHover(mContext, mouse);
                return true;
            }
            
            if(!mHovering){
                mHovering = true;
                if(mHandlers.mOnMouseIn && mEnabled)
                    mHandlers.mOnMouseIn(mContext, mouse);
            }
            
            return true;
            
        }else{
            if(mHovering){
                mHovering = false;
                if(mHandlers.mOnMouseOut && mEnabled)
                    mHandlers.mOnMouseOut(mContext, mouse);
            }
            return false;
        }
    
    }
    
    bool InputComponent::mouseExit(const ofMouseEventArgs& mouse)
    {
        if(mPressed || mHovering){
            mPressed = false;
            mHovering = false;
            return true;
        }
        return false;
    }
    
    bool InputComponent::mouseDragged(const ofMouseEventArgs& mouse)
    {
        //todo, if not pressed, does it pick it up?
        if(mPressed){
            mDragState.delta = mouse - mDragState.mouseStart;
            if(mHandlers.mOnMouseDragged && mEnabled)
                mHandlers.mOnMouseDragged(mContext, mouse, mDragState);
            return true;
        }
        return false;
    }
    
    bool InputComponent::mousePressed(const ofMouseEventArgs& mouse)
    {
        if(check(mouse)){
            if(!mPressed){
                mPressed = true;
                mHovering = false;
                mDragState.mouseStart = mouse;
                mDragState.positionStart = glm::vec2(mNode.lock()->getPosition());
                mDragState.delta = glm::vec2(0);
                if(mHandlers.mOnMousePressed && mEnabled)
                    mHandlers.mOnMousePressed(mContext, mouse);
                return true;
            }
        }
        return false;
    }
    
    bool InputComponent::mouseReleased(const ofMouseEventArgs& mouse)
    {
        if(mPressed){
            mPressed = false;
            if(mHandlers.mOnMouseReleased && mEnabled)
                mHandlers.mOnMouseReleased(mContext, mouse);
            if(check(mouse)){
                mHovering = true;
                if(mHandlers.mOnMouseHover && mEnabled)
                    mHandlers.mOnMouseHover(mContext, mouse);
            }
            return true;
        }
        return false;
    }
    
    bool InputComponent::mouseScrollWheel(const ofMouseEventArgs& mouse)
    {
        if((mPressed || mHovering) && mHandlers.mOnMouseScrollWheel && mEnabled){
            mHandlers.mOnMouseScrollWheel(mContext, mouse);
            return true;
        }
        return false;
    }
    
    void InputComponent::keyPressed(const ofKeyEventArgs& key)
    {
        if(mHandlers.mOnKeyPressed && mEnabled)
            mHandlers.mOnKeyPressed(mContext, key);
    }
    
    void InputComponent::keyReleased(const ofKeyEventArgs& key)
    {
        if(mHandlers.mOnKeyReleased && mEnabled)
            mHandlers.mOnKeyReleased(mContext, key);
    }
    
}//end namespace mediasystem
