//
//  RenderComponent.cpp
//  WallTest
//
//  Created by Michael Allison on 6/12/18.
//

#include "LayeredMediaComponent.h"
#include "mediasystem/core/Entity.hpp"
#include "ofMain.h"

namespace mediasystem {
    
    LayeredMediaComponent::LayeredMediaComponent(Entity& context, std::shared_ptr<MediaBase> media, RectMode mode ):
        mContext(context),
        mNode(context.getComponent<ofNode>()),
        mMedia(std::move(media)),
        mRectMode(mode)
    {
        mMedia->init();
        auto size = mMedia->getMediaSize();
        setRect(size.x, size.y, size.x, size.y);
    }
    
    LayeredMediaComponent::LayeredMediaComponent(Entity& context, std::shared_ptr<MediaBase> media, float width, float height, RectMode mode ):
        mContext(context),
        mNode(context.getComponent<ofNode>()),
        mMedia(std::move(media)),
        mRectMode(mode)
    {
        mMedia->init();
        auto size = mMedia->getMediaSize();
        setRect(width, height, size.x, size.y);
    }
    
    void LayeredMediaComponent::setRect(float width, float height, float mediaW, float mediaH)
    {
        switch(mRectMode){
            case RECT_MODE_CENTER:
            {
                mRect.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
                
                mRect.addVertex(glm::vec3(-width/2.f,-height/2.f, 0.f));
                mRect.addTexCoord(glm::vec2(0,0));
                
                mRect.addVertex(glm::vec3(width/2.f,-height/2.f, 0.f));
                mRect.addTexCoord(glm::vec2(mediaW,0));
                
                mRect.addVertex(glm::vec3(-width/2.f,height/2.f, 0.f));
                mRect.addTexCoord(glm::vec2(0,mediaH));
                
                mRect.addVertex( glm::vec3(width/2.f,height/2.f, 0.f));
                mRect.addTexCoord(glm::vec2(mediaW,mediaH));
                
            }break;
            case RECT_MODE_TOP_LEFT:
            {
                mRect.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
                
                mRect.addVertex(glm::vec3(0.f));
                mRect.addTexCoord(glm::vec2(0,0));
                
                mRect.addVertex(glm::vec3(width, 0.f, 0.f));
                mRect.addTexCoord(glm::vec2(mediaW,0));
                
                mRect.addVertex(glm::vec3(0,height, 0.f));
                mRect.addTexCoord(glm::vec2(0,mediaH));
                
                mRect.addVertex( glm::vec3(width,height, 0.f));
                mRect.addTexCoord(glm::vec2(mediaW,mediaH));
                
            }break;
        }
    }
    
    glm::mat4 LayeredMediaComponent::getGlobalTransform()
    {
        glm::mat4 ret;
        if(auto node = mNode.lock()){
            ret = node->getGlobalTransformMatrix();
        }
        return ret;
    }
    
    void LayeredMediaComponent::draw()
    {
        mMedia->bind();
        mRect.draw();
        mMedia->unbind();
    }
    
    void LayeredMediaComponent::debugDraw()
    {
        mMedia->debugDraw();
    }
    
    glm::vec2 LayeredMediaComponent::getMediaSize()
    {
        return mMedia->getMediaSize();
    }

    
}//end namespace mediasystem
