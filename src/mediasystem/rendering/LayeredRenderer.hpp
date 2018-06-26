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
#include "mediasystem/core/ComponentSystem.hpp"
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
        void setLayer(float layer){ mLayer = layer; }
        void hide(){ mVisible = false; }
        void show(){ mVisible = true; }
        void setColor(const ofFloatColor& color){ mColor = color; }
        void setAlpha(float alpha){ mAlpha = alpha; }
        
        //layered concept
        float getLayer() const { return mLayer; }
        bool isVisible()const{ return mVisible; }
        const ofFloatColor& getColor()const{ return mColor; }
        float getAlpha() const { return mAlpha; }
        glm::mat4 getGlobalTransformMatrix(){
            if(auto node = mEntity.getComponent<ofNode>()){
                return node->getGlobalTransformMatrix();
            }
            return glm::mat4();
        }
        
        //drawable concept
        void draw(){ T::draw(); }
        
    private:
        Entity& mEntity;
        ofFloatColor mColor{1.,1.,1.};
        float mAlpha{1};
        float mLayer{0.f};
        bool mVisible{true};
    };
    
    template<typename...DrawableTypes>
    class LayeredRenderer {
        
        using Layer = std::tuple<std::list<std::weak_ptr<Drawable<DrawableTypes>>>...>;
        using LayersMap = std::map<float, Layer>;
        
    public:
        
        LayeredRenderer(Scene& scene):
            mScene(scene),
            mPresenter( new DefaultPresenter )
        {
            mScene.addDelegate<Draw>(EventDelegate::create<LayeredRenderer,&LayeredRenderer::onDraw>(this));
            int l[] = {(addNewComponentDelegate<DrawableTypes>(),0)...};
            (void)l;
        }
        
        ~LayeredRenderer(){
            mScene.removeDelegate<Draw>(EventDelegate::create<LayeredRenderer,&LayeredRenderer::onDraw>(this));
            int l[] = {(removeNewComponentDelegate<DrawableTypes>(),0)...};
            (void)l;
        }
        
        template<typename Presenter, typename...Args>
        Presenter* createPresenter( Args&&...args ){
            static_assert(std::is_base_of<IPresenter,Presenter>::value, "Presenter must derive from IPresenter.");
            if(mPresenter)
                mPresenter.reset();
            mPresenter.reset(new Presenter(std::forward<Args>(args)...));
            return static_cast<Presenter*>(mPresenter.get());
        }
        
        void draw(){

            for ( auto & layer : mLayers ) {
                for_each_in_tuple(layer.second,LayerChecker<DrawableTypes...>(*this,layer.first));
            }

            mPresenter->begin();
            
            for ( auto & layer : mLayers ) {
                for_each_in_tuple(layer.second,LayerDrawer<DrawableTypes...>(*this));
            }
            
            mPresenter->end();
            mPresenter->present();
        }
        
        void setGlobalAlpha(float alpha){ mGlobalAlpha = alpha; }
        float getGlobalAlpha() const { return mGlobalAlpha; }
        
    private:
        
        template<typename...Args>
        struct LayerChecker {
            LayerChecker(LayeredRenderer<Args...>& renderer, float layer):mRenderer(renderer), mLayer(layer){}
            template<typename T>
            void operator ()(T&& t){
                mRenderer.checkLayer(mLayer, t);
            }
            LayeredRenderer<Args...>& mRenderer;
            float mLayer{0.f};
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
        void insertIntoLayer(float layer, std::weak_ptr<Drawable<T>> handle){
            auto found = mLayers.find(layer);
            if(found != mLayers.end()){
                auto& list = get_element_by_type<std::list<std::weak_ptr<Drawable<T>>>>(found->second);
                list.emplace_back(std::move(handle));
            }else{
                Layer l;
                auto& list = get_element_by_type<std::list<std::weak_ptr<Drawable<T>>>>(l);
                list.emplace_back(std::move(handle));
                mLayers.emplace(layer, std::move(l));
            }
        }
        
        template<typename T>
        void checkLayer(float layer, std::list<std::weak_ptr<Drawable<T>>>& handles){
            auto it = handles.begin();
            auto end = handles.end();
            while( it != end ){
                if(auto component = it->lock()){
                    if(component->getLayer() != layer){
                        it = handles.erase(it);
                        insertIntoLayer<T>(component->getLayer(), std::move(component));
                    }else{
                        ++it;
                    }
                }else{
                    it = handles.erase(it);
                }
            }
        }
        
        template<typename T>
        void drawLayer(std::list<std::weak_ptr<Drawable<T>>>& handles){
            auto it = handles.begin();
            auto end = handles.end();
            while (it!=end) {
                if (auto component = (*it).lock()) {
                    if (component->isVisible()) {
                        auto model = component->getGlobalTransformMatrix();
                        auto c = component->getColor();
                        auto a = component->getAlpha();
                        ofSetColor(ofFloatColor(c.r, c.g, c.b, a * mGlobalAlpha));
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
                    insertIntoLayer<T>(layer, std::move(compHandle));
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
        LayersMap mLayers;
        std::unique_ptr<IPresenter> mPresenter;
    };
}//end namespace
