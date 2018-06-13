#include "Scene.h"
#include "mediasystem/core/Entity.hpp"
#include "mediasystem/core/MediaController.h"
#include "mediasystem/util/Util.h"
#include "mediasystem/core/CoreEvents.h"

namespace mediasystem {

    Scene::Scene(MediaController& context, const std::string & name, float transition_time):
        mController(context),
        mName(name)
    {
        context.getEventManager().addDelegate<ShutdownSystem>(EventDelegate::create<Scene,&Scene::onShutdown>(this));
        context.getEventManager().addDelegate<InitSystem>(EventDelegate::create<Scene,&Scene::onInit>(this));
        context.getEventManager().addDelegate<PostInit>(EventDelegate::create<Scene,&Scene::onPostInit>(this));
    }

    Scene::~Scene()
    {
        mController.getEventManager().removeDelegate<ShutdownSystem>(EventDelegate::create<Scene,&Scene::onShutdown>(this));
        mController.getEventManager().removeDelegate<InitSystem>(EventDelegate::create<Scene,&Scene::onInit>(this));
        mController.getEventManager().removeDelegate<PostInit>(EventDelegate::create<Scene,&Scene::onPostInit>(this));
    }
    
    static size_t sNextEntityId = 0;
    
    EntityHandle Scene::createEntity()
    {
        auto next = sNextEntityId++;
        auto it = mEntities.emplace(next, EntityRef( new Entity(*this, next)));
        if(it.second){
            mController.getEventManager().queueEvent(std::make_shared<NewEntity>(it.first->second));
            //everyone gets a node component, because why not
            it.first->second->createComponent<ofNode>();
            return it.first->second;
        }else{
            return EntityHandle();
        }
    }
    
    bool Scene::destroyEntity(size_t id)
    {
        auto found = mEntities.find(id);
        if(found != mEntities.end()){
            mController.getEventManager().queueEvent(std::make_shared<DestroyEntity>(found->second));
            mEntities.erase(found);
            return true;
        }
        return false;
    }
    
    bool Scene::destroyEntity(EntityHandle handle)
    {
        if(auto ent = handle.lock()){
            return destroyEntity(ent->getId());
        }
    }
    
    EntityHandle Scene::getEntity(size_t id)
    {
        auto found = mEntities.find(id);
        if(found != mEntities.end()){
            return EntityHandle(found->second);
        }else{
            return EntityHandle();
        }
    }
    
    bool Scene::destroyComponent(type_id_t type, size_t entity_id){
        return mController.destroyComponent(type, entity_id);
    }
    
    float Scene::getPercentTransitionComplete() const
    {
        switch(mTransitionDirection){
            case TRANSITION_IN:
                return (ofGetElapsedTimef() - mTransitionStart) / mTransitionInDuration;
            case TRANSITION_OUT:
                return (ofGetElapsedTimef() - mTransitionStart) / mTransitionOutDuration;
            default: return 0;
        }
    }
    
    void Scene::init()
    {
        mEvents[INIT](this);
    }
    
    void Scene::postInit()
    {
        mEvents[POST_INIT](this);
    }
    
    void Scene::transitionIn()
    {
        mTransitionDirection = TRANSITION_IN;
        mIsTransitioning = true;
        mTransitionStart = ofGetElapsedTimef();
        mEvents[TRANSITION_IN_BEGIN](this);
        start();
    }
    
    void Scene::transitionOut()
    {
        mTransitionDirection = TRANSITION_OUT;
        mIsTransitioning = true;
        mTransitionStart = ofGetElapsedTimef();
        mEvents[TRANSITION_OUT_BEGIN](this);
    }
    
    void Scene::transitionInComplete()
    {
        mIsTransitioning = false;
        mEvents[TRANSITION_IN_END](this);
    }
    
    void Scene::transitionOutComplete()
    {
        mIsTransitioning = false;
        mEvents[TRANSITION_OUT_END](this);
        stop();
    }
    
    void Scene::update()
    {
        auto self = this;
        if(mIsTransitioning){
            auto perc = getPercentTransitionComplete();
            if(perc >= 1.){
                switch(mTransitionDirection){
                    case TRANSITION_IN:
                        transitionInComplete();
                        return;
                    case TRANSITION_OUT:
                        transitionOutComplete();
                        return;
                }
            }else{
                mEvents[TRANSITION_UPDATE](this);
            }
        }
        mEvents[UPDATE](this);
    }
    
    void Scene::start()
    {
        mHasStarted = true;
        mEvents[START](this);
    }

    void Scene::stop()
    {
        mHasStarted = false;
        mEvents[STOP](this);
    }
    
    void Scene::addDelegate(Scene::Events event, SceneEventDelegate delegate)
    {
        mEvents[event] += delegate;
    }
    
    void Scene::removeDelegate(Scene::Events event, SceneEventDelegate delegate)
    {
        mEvents[event] -= delegate;
    }
    
    float Scene::getTransitionDuration(Transition direction)const
    {
        switch(direction){
            case TRANSITION_IN:
                return mTransitionInDuration;
            case TRANSITION_OUT:
                return mTransitionOutDuration;
        }
        return 0.f;
    }
    
    void Scene::setTransitionDuration(Transition direction, float duration)
    {
        switch(direction){
            case TRANSITION_IN:
                mTransitionInDuration = duration;
                return;
            case TRANSITION_OUT:
                mTransitionOutDuration = duration;
                return;
        }
    }
    
    void Scene::shutdown()
    {
        mEntities.clear();
    }
    
    void Scene::onInit(const IEventRef& event)
    {
        init();
    }
    
    void Scene::onPostInit(const IEventRef& event)
    {
        postInit();
    }
    
    void Scene::onShutdown(const IEventRef& event)
    {
        shutdown();
    }

}//end namespace mediasystem
