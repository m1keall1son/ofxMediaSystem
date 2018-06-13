//
//  LayeredMediaSystem.cpp
//  WallTest
//
//  Created by Michael Allison on 6/11/18.
//

#include "LayeredMediaSystem.h"
#include "mediasystem/core/CoreEvents.h"
#include "mediasystem/util/Log.h"

namespace mediasystem {
    
    LayeredMediaSystem::LayeredMediaSystem(EventManager& eventManager):
        mEventManager(eventManager)
    {
        mEventManager.addDelegate<DrawEvent>(EventDelegate::create<LayeredMediaSystem,&LayeredMediaSystem::onDrawEvent>(this));
        mEventManager.addDelegate<NewComponent<LayeredMediaComponent>>(EventDelegate::create<LayeredMediaSystem,&LayeredMediaSystem::onNewComponentEvent>(this));
    }
    
    void LayeredMediaSystem::onDrawEvent(const IEventRef& event)
    {
        draw();
    }
    
    void LayeredMediaSystem::onNewComponentEvent(const IEventRef& event)
    {
        auto cast = std::static_pointer_cast<NewComponent<LayeredMediaComponent>>(event);
        if(cast->getComponentType() == type_id<LayeredMediaComponent>){
            auto compHandle = cast->getComponentHandle();
            if(auto comp = compHandle.lock()){
                auto layer = comp->getLayer();
                insertIntoLayer(layer, std::move(compHandle));
            }
        }else{
            MS_LOG_ERROR("Something is wrong, the type_ids should match for in layered media system on new component!");
        }
    }
    
    void LayeredMediaSystem::draw()
    {
        { // check if any layers changed
            auto layerIt = mComponentsByLayer.begin();
            auto layersEnd = mComponentsByLayer.end();
            while(layerIt != layersEnd){
                auto it = layerIt->second.begin();
                auto end = layerIt->second.end();
                while( it != end ){
                    if(auto component = it->lock()){
                        if(component->getLayer() != layerIt->first){
                            it = layerIt->second.erase(it);
                            insertIntoLayer(component->getLayer(), component);
                        }else{
                            ++it;
                        }
                    }else{
                        it = layerIt->second.erase(it);
                    }
                }
                ++layerIt;
            }
        }
        
        mPresenter->begin();
        
        for ( auto & layer : mComponentsByLayer ) {
            auto it = layer.second.begin();
            auto end = layer.second.end();
            while (it!=end) {
                if (auto component = (*it).lock()) {
                    if (component->isVisible()) {
                        auto model = component->getGlobalTransform();
                        auto c = component->getColor();
                        auto a = component->getAlpha();
                        ofSetColor(ofFloatColor(c.r, c.g, c.b, a));
                        ofPushMatrix();
                        ofMultMatrix(model);
                        component->draw();
                        if(isDebugDrawEnabled())
                            component->debugDraw();
                        ofPopMatrix();
                        //TODO how to view different types of debug information?
                        //if(behavior->allowsInput() && mDebug && behavior->isDebugDrawEnabled())
                        //behavior->drawInputDebug();
                    }
                    ++it;
                }
                else {
                    it = layer.second.erase(it);
                }
            }
        }
        
        mPresenter->end();
        
        mPresenter->present();
    }
    
    void LayeredMediaSystem::insertIntoLayer(float layer, LayeredMediaComponentHandle&& handle)
    {
        auto found = mComponentsByLayer.find(layer);
        if(found != mComponentsByLayer.end()){
            found->second.emplace_back(std::move(handle));
        }else{
            std::list<LayeredMediaComponentHandle> list = {std::move(handle)};
            mComponentsByLayer.emplace( layer, std::move(list) );
        }
    }
    
    void LayeredMediaSystem::clear()
    {
        mComponentsByLayer.clear();
    }

}//end namespace media system

