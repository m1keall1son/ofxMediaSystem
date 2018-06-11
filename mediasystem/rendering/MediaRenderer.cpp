//
//  MediaRenderer.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#include "MediaRenderer.h"
#include "mediasystem/behaviors/ViewBase.h"
#include "ofMain.h"

namespace mediasystem {
    
    MediaRenderer::MediaRenderer()
    {
        
    }
    
    MediaRenderer::~MediaRenderer()
    {
        
    }
    
    void MediaRenderer::draw()
    {
        { // check if any layers changed
            auto layerIt = mDrawQueue.begin();
            auto layersEnd = mDrawQueue.end();
            while(layerIt != layersEnd){
                auto it = layerIt->second.begin();
                auto end = layerIt->second.end();
                while( it != end ){
                    if(auto behavior = it->lock()){
                        if(behavior->getLayer() != layerIt->first){
                            it = layerIt->second.erase(it);
                            insert(behavior->getLayer(), std::weak_ptr<ViewBase>(behavior));
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
    
        for ( auto & layer : mDrawQueue ) {
            auto it = layer.second.begin();
            auto end = layer.second.end();
            while (it!=end) {
                if (auto behavior = (*it).lock()) {
                    if (behavior->isEnabled() && behavior->isVisible()) {
                        auto model = behavior->getTransform();
                        auto c = behavior->getColor();
                        auto a = behavior->getAlpha();
                        ofSetColor(ofFloatColor(c.r, c.g, c.b, a));
                        ofPushMatrix();
                        ofMultMatrix(model);
                        behavior->draw();
                        if(mDebug && behavior->isDebugDrawEnabled())
                            behavior->debugDraw();
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
    
    void MediaRenderer::insert(float layer, const ViewBaseHandle& handle)
    {
        auto found = mDrawQueue.find(layer);
        if(found != mDrawQueue.end()){
            found->second.push_back(handle);
        }else{
            mDrawQueue.insert({ layer, {handle} });
        }
    }
    
    void MediaRenderer::clear(){ mDrawQueue.clear(); }

    
}//end namespace mediasystem
