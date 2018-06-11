#include "ViewBase.h"
#include "mediasystem/core/SceneBase.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {

    ViewBase::ViewBase(SceneBase* scene) :
        IBehavior(scene),
        mAlpha(1.0f),
        mScale(glm::vec3(1)),
        mPosition(glm::vec3(0)),
        mColor(ofFloatColor(1.f, 1.f, 1.f)),
        mDirtyTransform(true),
        mLayer(0.0f)
    {}

    const glm::mat4 & ViewBase::getTransform()
    {
        if(mDirtyTransform){
            mCachedTransform =
                glm::translate(mPosition + mAnchor) *
                glm::toMat4(mRotation) *
                glm::scale(mScale) *
                glm::translate(-mAnchor);
            mDirtyTransform = false;
            if(mInputComponent)
                mInputComponent->update();
        }
        return mCachedTransform;
    }

    //Dealing with state
    void ViewBase::setAlpha(const float alpha) { mAlpha = alpha; }
    float ViewBase::getAlpha() const { return mAlpha; }

     void ViewBase::setPosition(const glm::vec3& position)
     {
         mPosition = position;
         mDirtyTransform = true;
     }

     void ViewBase::setPosition(const glm::vec2& position)
     {
         mPosition = glm::vec3(position, mPosition.z);
         mDirtyTransform = true;
     }

     void ViewBase::setPosition(const float x, const float y) {
         mPosition = glm::vec3(x, y, mPosition.z);
         mDirtyTransform = true;
     }

    glm::vec3 ViewBase::getPosition() const { return mPosition; }

    void ViewBase::setAnchor(const glm::vec3& localPosition)
    {
        mAnchor = localPosition;
        mDirtyTransform = true;
    }

    void ViewBase::setAnchor(const glm::vec2& anchor)
    {
        mAnchor = glm::vec3(anchor, mAnchor.z);
        mDirtyTransform = true;
    }

    void ViewBase::setAnchor(const float x, const float y)
    {
        mAnchor = glm::vec3(x, y, mAnchor.z);
        mDirtyTransform = true;
    }

    glm::vec3 ViewBase::getAnchor() const {  return mAnchor; }

    void ViewBase::setScale(const glm::vec3& scale)
    {
        mScale = scale;
        mDirtyTransform = true;
    }
    
    void ViewBase::setScale(float scale)
    {
        mScale = glm::vec3(scale);
        mDirtyTransform = true;
    }
    
    void ViewBase::setScale(const glm::vec2& scale)
    {
        mScale = glm::vec3(scale, mScale.z);
        mDirtyTransform = true;
    }

    void ViewBase::setScale(const float x, const float y)
    {
        mScale = glm::vec3(x, y, mScale.z);
        mDirtyTransform = true;
    }

    glm::vec3 ViewBase::getScale() const { return mScale; }

    void ViewBase::setRotation(const glm::quat& rot)
    {
        mRotation = rot;
        mDirtyTransform = true;
    }
    
    void ViewBase::setRotation(float angle, const glm::vec3& axis)
    {
        mRotation = glm::toQuat(glm::rotate(angle, axis));
        mDirtyTransform = true;
    }
    
    void ViewBase::setRotationEuler(float x, float y, float z)
    {
        mRotation = glm::toQuat(glm::eulerAngleXYZ(x, y, z));
        mDirtyTransform = true;
    }

    glm::quat ViewBase::getRotation() const { return mRotation; }
    glm::vec3 ViewBase::getRotationEuler() const { return glm::eulerAngles(mRotation);}
    
    void ViewBase::setColor(const ofFloatColor& color) { mColor = color; }
    ofFloatColor ViewBase::getColor() const { return mColor; }
    
    void ViewBase::allowInput( const ofRectangle& activeScreenArea, int zIndex, const InputHandlers& handlers )
    {
        mInputComponent = std::make_shared<InputComponent>(this, activeScreenArea, zIndex);
        mInputComponent->setOnMouseInHandler(handlers.mOnMouseIn);
        mInputComponent->setOnMouseOutHandler(handlers.mOnMouseOut);
        mInputComponent->setOnMouseHoverHandler(handlers.mOnMouseHover);
        mInputComponent->setOnMousePressedHandler(handlers.mOnMousePressed);
        mInputComponent->setOnMouseReleasedHandler(handlers.mOnMouseReleased);
        mInputComponent->setOnMouseScrollWheelHandler(handlers.mOnMouseScrollWheel);
        mInputComponent->setOnMouseDraggedHandler(handlers.mOnMouseDragged);
        mInputComponent->setOnKeyPressedHandler(handlers.mOnKeyPressed);
        mInputComponent->setOnKeyReleasedHandler(handlers.mOnKeyReleased);
    }
    
    void ViewBase::drawInputDebug()
    {
        //push
        auto style = ofGetStyle();
        auto col = style.color;
        auto fill = style.bFill;
        
        auto& screenArea = mInputComponent->getScreenArea();
        
        auto wire = ofColor(255,255,0);
        
        if(mInputComponent->isHovering()){
            wire = ofColor(255,127,0);
        }else if(mInputComponent->isPressed()){
            wire = ofColor(255,0,0);
        }
        
        //draw
        ofSetColor(wire);
        ofNoFill();
        ofDrawRectangle(screenArea);
        auto font = getDebugFont();
        auto extra = font->getSize() + font->getDescenderHeight() + font->getAscenderHeight();
        auto padding = 2;
        
        //current type
        ofFill();
        ofSetColor(50, 50, 50);
        auto viewName = "INPUT AREA";
        
        auto sw = font->stringWidth(viewName);
        auto line = screenArea.y;
        
        ofDrawRectangle(screenArea.x, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 0);
        font->drawStringAsShapes(viewName, screenArea.x + padding, line + padding + font->getAscenderHeight() );
        
        //path
        ofFill();
        ofSetColor(0, 0, 0);
        std::string state = "state: ";
        
        if(mInputComponent->isPressed()){
            state += "PRESSED";
        }else if(mInputComponent->isHovering()){
            state += "HOVER";
        }else{
            state += "IDLE";
        }
        
        sw = font->stringWidth(state);
        line = screenArea.y + extra + (padding * 2);
        
        ofDrawRectangle(screenArea.x, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(wire);
        font->drawStringAsShapes(state, screenArea.x + padding, line + padding + font->getAscenderHeight() );
        
        //size
        ofFill();
        ofSetColor(0, 0, 0);
        auto area = "[x: "+ofToString(screenArea.x)+" y: "+ofToString(screenArea.y)+" w: "+ofToString(screenArea.width)+" h: "+ofToString(screenArea.height)+"]";
        
        sw = font->stringWidth(area);
        line = screenArea.y + (extra + (padding * 2))*2;
        
        ofDrawRectangle(screenArea.x, line, sw + (padding*2), (padding*2) + extra);
        ofSetColor(255, 255, 255);
        font->drawStringAsShapes(area, screenArea.x + padding, line + padding + font->getAscenderHeight() );
        
        //pop
        ofSetColor(col);
        if(fill)
            ofFill();
        else
            ofNoFill();
    }
    
}//end namespace mediasystem
