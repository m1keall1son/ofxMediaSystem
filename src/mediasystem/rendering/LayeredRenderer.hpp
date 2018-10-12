//
//  LayeredRenderer.hpp
//  WallTest
//
//  Created by Michael Allison on 6/19/18.
//

#pragma once

#include <tuple>
#include "ofMain.h"
#include "mediasystem/core/Entity.h"
#include "mediasystem/rendering/IPresenter.h"
#include "mediasystem/rendering/DefaultPresenter.h"
#include "mediasystem/util/TupleHelpers.hpp"

namespace mediasystem {

template<typename T>
class Drawable : public T {
public:
    
    template<typename...Args>
    Drawable(Entity& entity, Args&&...args):
        T(std::forward<Args>(args)...),
        mEntity(entity)
    {}
    
    //setters
    void setLayer(std::string layer){ mLayer = std::move(layer); }
    void setDrawOrder(float order){ mDrawOrder = order; }
    void hide(){ mVisible = false; }
    void show(){ mVisible = true; }
    void setColor(const ofFloatColor& color){ mColor = ofFloatColor(color.r,color.g,color.b,mColor.a); }
    void setAlpha(float alpha){ mColor.a = alpha; }
    
    //layered concept
    const std::string& getLayer() const { return mLayer; }
    float getDrawOrder() const { return mDrawOrder; }
    bool isVisible()const{ return mVisible; }
    const ofFloatColor& getColor()const{ return mColor; }
    ofFloatColor* getColorPtr(){ return &mColor; }
    float getAlpha() const { return mColor.a; }
    float* getAlphaPtr() { return &mColor.a; }
    glm::mat4 getGlobalTransformMatrix(){
        if(auto node = mEntity.getComponent<ofNode>()){
            return node->getGlobalTransformMatrix();
        }
        return glm::mat4();
    }
    
    //drawable concept
    void draw(){
        T::draw(); //must have a draw function
    }
    
private:
    Entity& mEntity;
    ofFloatColor mColor{1.,1.,1.,1.};
    float mDrawOrder{0.f};
    std::string mLayer{"default"};
    bool mVisible{true};
};
    
template<typename T>
using DrawableHandle = Handle<Drawable<T>>;

template<typename T>
using DrawableHandleList = std::list<DrawableHandle<T>,Allocator<DrawableHandle<T>>>;

template<typename...DrawableTypes>
class LayeredRenderer {
    
    using TypesList = std::tuple<DrawableHandleList<DrawableTypes>...>;
    using OrderedLayer = std::map<float /* draw order */, TypesList>;
    struct Layer {
        Layer( std::string _name, std::shared_ptr<IPresenter> _presenter, float order = 0.f ):
            name(std::move(_name)),
            presenter(std::move(_presenter))
        {}
        std::string name;
        float order{0.f};
        OrderedLayer layer;
        std::shared_ptr<IPresenter> presenter;
    };
    using LayerList = std::list<Layer>;
    
public:
    
    LayeredRenderer(Scene& scene):
    mScene(scene)
    {
        mLayers.emplace_back("default", std::make_shared<DefaultPresenter>(), std::numeric_limits<float>::max());
        mScene.addDelegate<Draw>(EventDelegate::create<LayeredRenderer,&LayeredRenderer::onDraw>(this));
        int l[] = {(addNewComponentDelegate<DrawableTypes>(),0)...};
        UNUSED_VARIABLE(l);
    }
    
    ~LayeredRenderer(){
        mScene.removeDelegate<Draw>(EventDelegate::create<LayeredRenderer,&LayeredRenderer::onDraw>(this));
        int l[] = {(removeNewComponentDelegate<DrawableTypes>(),0)...};
        UNUSED_VARIABLE(l);
    }

    void addLayer(std::string name, std::shared_ptr<IPresenter> presenter = std::make_shared<DefaultPresenter>(), float order = 0.f){
        auto found = std::find_if(mLayers.begin(), mLayers.end(), [order](const Layer& layer){
            return layer.order >= order;
        });
        if(found != mLayers.end()){
            mLayers.emplace(found, std::move(name), std::move(presenter), order);
        }else{
            mLayers.emplace_back(std::move(name), std::move(presenter), order);
        }
    }
    
    void setDefault(std::shared_ptr<IPresenter> presenter, float order = std::numeric_limits<float>::max()){
        auto found = std::find_if(mLayers.begin(), mLayers.end(), [](const Layer& layer){
            return layer.name == "default";
        });
        found->presenter = presenter;
        found->order = order;
    }
    
    void draw(){
        for(auto & layer : mLayers){
            for ( auto & order : layer.layer ) {
                for_each_in_tuple(order.second,OrderChecker<DrawableTypes...>(*this, layer.name, order.first));
            }
        }
        for(auto & layer : mLayers){
            layer.presenter->begin();
            for ( auto & order : layer.layer ) {
                for_each_in_tuple(order.second,LayerDrawer<DrawableTypes...>(*this));
            }
            layer.presenter->end();
        }
        for(auto & layer : mLayers){
            layer.presenter->present();
        }
    }
    
    void setGlobalAlpha(float alpha){ mGlobalAlpha = alpha; }
    float getGlobalAlpha() const { return mGlobalAlpha; }
    
private:
    
    template<typename...Args>
    struct OrderChecker {
        OrderChecker(LayeredRenderer<Args...>& renderer, std::string layer, float order):mRenderer(renderer), mLayer(std::move(layer)), mOrder(order){}
        template<typename T>
        void operator ()(T&& t){
            mRenderer.checkOrder(mLayer, mOrder, t);
        }
        LayeredRenderer<Args...>& mRenderer;
        std::string mLayer;
        float mOrder{0.f};
    };
    
    template<typename...Args>
    struct LayerDrawer {
        LayerDrawer(LayeredRenderer<Args...>& renderer):mRenderer(renderer){}
        template<typename T>
        void operator ()(T&& t){
            mRenderer.drawLayer(t);
        }
        LayeredRenderer<Args...>& mRenderer;
    };
    
    template<typename T>
    void addNewComponentDelegate(){
        mScene.addDelegate<NewComponent<Drawable<T>>>(EventDelegate::create<LayeredRenderer, &LayeredRenderer::onNewLayeredComponent<T>>(this));
    }
    
    template<typename T>
    void removeNewComponentDelegate(){
        mScene.removeDelegate<NewComponent<Drawable<T>>>(EventDelegate::create<LayeredRenderer, &LayeredRenderer::onNewLayeredComponent<T>>(this));
    }
    
    template<typename T>
    void insertIntoOrderedLayer(const std::string& layerName, float order, DrawableHandle<T> handle){
        auto found = std::find_if(mLayers.begin(), mLayers.end(), [&layerName](const Layer& layer){
            return layer.name == layerName;
        });
        if(found != mLayers.end()){
            //we have this layer, pull the typed list from the tuple at a given draw order
            auto foundOrder = found->layer.find(order);
            if(foundOrder != found->layer.end()){
                auto& list = get_element_by_type<DrawableHandleList<T>>(foundOrder->second);
                list.emplace_back(std::move(handle));
            }else{
                TypesList l{mScene.getAllocator<DrawableHandleList<DrawableTypes>>()...};
                auto& list = get_element_by_type<DrawableHandleList<T>>(l);
                list.emplace_back(std::move(handle));
                found->layer.emplace(order, std::move(l));
            }
            
        }else{
            //error and put it in default
            auto& defaultLayer = *(std::find_if(mLayers.begin(), mLayers.end(), [](const Layer& layer){
                return layer.name == "default";
            }));
            
            auto foundOrder = defaultLayer.layer.find(order);
            if(foundOrder != defaultLayer.layer.end()){
                auto& list = get_element_by_type<DrawableHandleList<T>>(foundOrder->second);
                list.emplace_back(std::move(handle));
            }else{
                TypesList l{mScene.getAllocator<DrawableHandleList<DrawableTypes>>()...};
                auto& list = get_element_by_type<DrawableHandleList<T>>(l);
                list.emplace_back(std::move(handle));
                defaultLayer.layer.emplace(order, std::move(l));
            }
            
            MS_LOG_ERROR("Didnt have a rendering layer called: " + layerName + " placeing drawable in default layer.");
        }
    }
    
    template<typename T>
    void checkOrder(const std::string& layer, float order, DrawableHandleList<T>& handles){
        auto it = handles.begin();
        auto end = handles.end();
        while( it != end ){
            if(auto component = it->lock()){
                if(component->getDrawOrder() != order || component->getLayer() != layer){
                    it = handles.erase(it);
                    insertIntoOrderedLayer<T>(component->getLayer(), component->getDrawOrder(), std::move(component));
                }else{
                    ++it;
                }
            }else{
                it = handles.erase(it);
            }
        }
    }
    
    template<typename T>
    void drawLayer(DrawableHandleList<T>& handles){
        auto it = handles.begin();
        auto end = handles.end();
        while (it!=end) {
            if (auto component = (*it).lock()) {
                if (component->isVisible()) {
                    auto model = component->getGlobalTransformMatrix();
                    auto c = component->getColor();
                    c.a *= mGlobalAlpha;
                    ofSetColor(c);
                    ofPushMatrix();
                    ofMultMatrix(model);
                    component->draw();
                    ofPopMatrix();
                }
                ++it;
            }
            else {
                it = handles.erase(it);
            }
        }
    }
    
    template<typename T>
    EventStatus onNewLayeredComponent( const IEventRef& event ){
        auto cast = std::static_pointer_cast<NewComponent<Drawable<T>>>(event);
        if(cast->getComponentType() == &type_id<Drawable<T>>){
            auto compHandle = cast->getComponentHandle();
            if(auto comp = compHandle.lock()){
                auto layer = comp->getLayer();
                auto order = comp->getDrawOrder();
                insertIntoOrderedLayer<T>(layer, order, std::move(compHandle));
            }
            return EventStatus::SUCCESS;
        }
        MS_LOG_ERROR("Something is wrong, the type_ids should match on new component event!");
        return EventStatus::FAILED;
    }
    
    EventStatus onDraw( const IEventRef& event ){
        draw();
        return EventStatus::SUCCESS;
    }
    
    float mGlobalAlpha{1.f};
    Scene& mScene;
    LayerList mLayers;
};
    
}//end namespace mediasystem

