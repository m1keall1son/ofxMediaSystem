//
//  InputComponent.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#include "InputComponent.h"
#include "mediasystem/behaviors/ViewBase.h"

namespace mediasystem {
    
    InputComponent::InputComponent( ViewBase* context, const ofRectangle& screenBounds, int z_index ) :
        mContext(context),
        mScreenBounds(screenBounds),
        mZIndex(z_index),
        mSize(screenBounds.width, screenBounds.height)
    {}
    
    void InputComponent::update()
    {
        auto pos = mContext->getPosition();
        auto scale = mContext->getScale();
        mScreenBounds = ofRectangle( mContext->getPosition().x, mContext->getPosition().y, mSize.x * scale.x, mSize.y * scale.y );
    }
    
    bool InputComponent::isEnabled()const
    {
        return mContext->isEnabled();
    }
    
    InputComponentHandle InputComponent::getHandle()
    {
        return InputComponentHandle(shared_from_this());
    }
    
    bool InputComponent::check(const glm::vec2& point)const
    {
        return mScreenBounds.inside(point);
    }
    
    bool InputComponent::mouseMove(const ofMouseEventArgs& mouse)
    {
    
        if(check(mouse)){
            
            if(mHovering){
                if(mOnMouseHover)
                    mOnMouseHover(mContext, mouse);
                return true;
            }
            
            if(!mHovering){
                mHovering = true;
                if(mOnMouseIn)
                    mOnMouseIn(mContext, mouse);
            }
            
            return true;
            
        }else{
            if(mHovering){
                mHovering = false;
                if(mOnMouseOut)
                    mOnMouseOut(mContext, mouse);
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
        if(mPressed && mOnMouseDragged){
            mDragState.delta = mouse - mDragState.mouseStart;
            mOnMouseDragged(mContext, mouse, mDragState);
            return true;
        }
    }
    
    bool InputComponent::mousePressed(const ofMouseEventArgs& mouse)
    {
        if(check(mouse)){
            if(!mPressed){
                mPressed = true;
                mHovering = false;
                mDragState.mouseStart = mouse;
                mDragState.viewPositionStart = glm::vec2(mContext->getPosition());
                mDragState.delta = glm::vec2(0);
                if(mOnMousePressed)
                    mOnMousePressed(mContext, mouse);
                return true;
            }
        }
        return false;
    }
    
    bool InputComponent::mouseReleased(const ofMouseEventArgs& mouse)
    {
        if(mPressed){
            mPressed = false;
            if(mOnMouseReleased)
                mOnMouseReleased(mContext, mouse);
            if(check(mouse)){
                mHovering = true;
                if(mOnMouseHover)
                    mOnMouseHover(mContext, mouse);
            }
            return true;
        }
        return false;
    }
    
    bool InputComponent::mouseScrollWheel(const ofMouseEventArgs& mouse)
    {
        if((mPressed || mHovering) && mOnMouseScrollWheel ){
            mOnMouseScrollWheel(mContext, mouse);
            return true;
        }
        return false;
    }
    
    void InputComponent::keyPressed(const ofKeyEventArgs& key)
    {
        if(mOnKeyPressed)
            mOnKeyPressed(mContext, key);
    }
    
    void InputComponent::keyReleased(const ofKeyEventArgs& key)
    {
        if(mOnKeyReleased)
            mOnKeyReleased(mContext, key);
    }
    
}//end namespace mediasystem
