#pragma once

#include "ofMain.h"
#include "mediasystem/behaviors/IBehavior.h"
#include "mediasystem/input/InputComponent.h"

namespace mediasystem {

	class SceneBase;
    
    class ViewBase;
    using ViewBaseRef = std::shared_ptr<ViewBase>;
    using ViewBaseHandle = std::weak_ptr<ViewBase>;
    
    class InputHandlers {
    public:
        
        InputHandlers() = default;
        ~InputHandlers() = default;
        
        inline InputHandlers& mouseInHandler(const MouseInputHandler& handler){ mOnMouseIn = handler; return *this; }
        inline InputHandlers& mouseOutHandler(const MouseInputHandler& handler){ mOnMouseOut = handler; return *this; }
        inline InputHandlers& mouseHoverHandler(const MouseInputHandler& handler){ mOnMouseHover = handler; return *this; }
        inline InputHandlers& mousePressedHandler(const MouseInputHandler& handler){ mOnMousePressed = handler; return *this; }
        inline InputHandlers& mouseReleasedHandler(const MouseInputHandler& handler){ mOnMouseReleased = handler; return *this; }
        inline InputHandlers& mouseDraggedHandler(const MouseDragHandler& handler){ mOnMouseDragged = handler; return *this; }
        inline InputHandlers& mouseScrollWheelHandler(const MouseInputHandler& handler){ mOnMouseScrollWheel = handler; return *this; }
        inline InputHandlers& keyPressedHandler(const KeyboardInputHandler& handler){ mOnKeyPressed = handler; return *this; }
        inline InputHandlers& keyReleasedHandler(const KeyboardInputHandler& handler){ mOnKeyReleased = handler; return *this; }
        
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
        friend class ViewBase;
    };

    class ViewBase : public IBehavior {
	public:
        
		ViewBase(SceneBase* scene);
		virtual ~ViewBase() = default;
        
        inline virtual void setLayer( float layer ) { mLayer = layer; }
        inline virtual float getLayer() const { return mLayer; }
        
        inline virtual void show() { mVisible = true; }
        inline virtual void hide() { mVisible = false; }
        inline virtual bool isVisible()const { return mVisible; }

		virtual const glm::mat4& getTransform();
		
		virtual void setAlpha(const float alpha);
		virtual float getAlpha()const;

		virtual void setPosition(const glm::vec3& position);
		virtual void setPosition(const glm::vec2& position);
		virtual void setPosition(const float x, const float y);
		virtual glm::vec3 getPosition()const;

		virtual void setAnchor(const glm::vec3& anchor);
		virtual void setAnchor(const glm::vec2& anchor);
		virtual void setAnchor(const float x, const float y);
		virtual glm::vec3 getAnchor()const;

		virtual void setScale(const glm::vec3& scale);
		virtual void setScale(const glm::vec2& scale);
        virtual void setScale(float scale);
		virtual void setScale(const float x, const float y);
		virtual glm::vec3 getScale()const;

		virtual void setRotation(const glm::quat& rot);
        virtual void setRotation(float angle, const glm::vec3& axis);
        virtual void setRotationEuler(float x, float y, float z);
		virtual glm::quat getRotation()const;
        virtual glm::vec3 getRotationEuler()const;

		virtual void setColor(const ofFloatColor& color);
		virtual ofFloatColor getColor()const;
        
        virtual void allowInput( const ofRectangle& activeScreenArea, int zIndex, const InputHandlers& handlers );
        inline bool allowsInput()const{ return mInputComponent ? true : false; }
        inline InputComponentHandle getInputComponent(){ return mInputComponent ? mInputComponent->getHandle() : InputComponentHandle(); }
		
        void drawInputDebug();
        
	private:

        bool                mVisible{true};
        float               mLayer;
		glm::vec3	        mAnchor;
		glm::vec3        	mPosition;
		glm::vec3        	mScale;
		glm::quat	        mRotation;
		ofFloatColor        mColor;
		float		        mAlpha;
        bool				mDirtyTransform{true};
		glm::mat4			mCachedTransform;
        std::shared_ptr<InputComponent> mInputComponent;

	};

}//end namespace mediasystem
