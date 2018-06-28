#include "Scene.h"
#include "mediasystem/core/Entity.h"
#include "mediasystem/util/Util.h"

namespace mediasystem {

    Scene::Scene(const std::string & name, float transition_duration):
        mName(name),
        mTransitionInDuration(transition_duration),
        mTransitionOutDuration(transition_duration)
    {}
    
    Scene::Scene(const std::string & name, float transition_in_duration, float transition_out_duration ):
        mName(name),
        mTransitionInDuration(transition_in_duration),
        mTransitionOutDuration(transition_out_duration)
    {}

    Scene::~Scene()
    {}
    
    static size_t sNextEntityId = 0;
    
    EntityHandle Scene::createEntity()
    {
        auto next = sNextEntityId++;
        auto it = mEntities.emplace(next, EntityRef( new Entity(*this, next)));
        if(it.second){
            queueEvent<NewEntity>(it.first->second);
            //everyone gets a node component, because why not
            it.first->second->createComponent<ofNode>();
            it.first->second->createComponent<EntityGraph>(*it.first->second);
            return it.first->second;
        }else{
            return EntityHandle();
        }
    }
    
    bool Scene::destroyEntity(size_t id)
    {
        auto found = mEntities.find(id);
        if(found != mEntities.end()){
            queueEvent<DestroyEntity>(found->second);
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
        return false;
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
        return mComponentManager.destroy(type, entity_id);
    }
    
    std::weak_ptr<void> Scene::getComponent(type_id_t type, size_t entity_id)
    {
        return mComponentManager.retrieve(type, entity_id);
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
    
    void Scene::notifyTransitionIn()
    {
        if(mTransitionInDuration > 0){
            mTransitionDirection = TRANSITION_IN;
            mIsTransitioning = true;
            mTransitionStart = ofGetElapsedTimef();
            transitionIn();
            triggerEvent<TransitionInBegin>(*this);
        }
        notifyStart();
    }
    
    void Scene::notifyTransitionOut()
    {
        if(mTransitionOutDuration > 0){
            mTransitionDirection = TRANSITION_OUT;
            mIsTransitioning = true;
            mTransitionStart = ofGetElapsedTimef();
            transitionOut();
            triggerEvent<TransitionOutBegin>(*this);
        }else{
            notifyStop();
        }
    }
    
    void Scene::notifyTransitionInComplete()
    {
        mIsTransitioning = false;
        transitionInComplete();
        triggerEvent<TransitionInComplete>(*this);
    }
    
    void Scene::notifyTransitionOutComplete()
    {
        mIsTransitioning = false;
        transitionOutComplete();
        triggerEvent<TransitionOutComplete>(*this);
        notifyStop();
    }
    
    void Scene::notifyUpdate()
    {
        auto self = this;
        if(mIsTransitioning){
            auto perc = getPercentTransitionComplete();
            if(perc >= 1.){
                switch(mTransitionDirection){
                    case TRANSITION_IN:
                        notifyTransitionInComplete();
                        return;
                    case TRANSITION_OUT:
                        notifyTransitionOutComplete();
                        return;
                }
            }else{
                transitionUpdate();
                triggerEvent<TransitionUpdate>(*this);
            }
        }
        auto now = ofGetElapsedTimef();
        update();
        triggerEvent<Update>(*this,ofGetFrameNum(),now, now - mLastUpdateTime);
        mLastUpdateTime = now;
        //process any events queued by other systems and components, etc.
        processEvents();
    }
    
    void Scene::notifyStart()
    {
        mHasStarted = true;
        start();
        triggerEvent<Start>(*this);
    }

    void Scene::notifyStop()
    {
        mHasStarted = false;
        mIsTransitioning = false;
        stop();
        triggerEvent<Stop>(*this);
    }
    
    float Scene::getTransitionDuration(TransitionDir direction)const
    {
        switch(direction){
            case TRANSITION_IN:
                return mTransitionInDuration;
            case TRANSITION_OUT:
                return mTransitionOutDuration;
        }
        return 0.f;
    }
    
    void Scene::setTransitionDuration(TransitionDir direction, float duration)
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
    
    void Scene::notifyInit()
    {
        init();
        triggerEvent<Init>(*this);
    }
    
    void Scene::notifyPostInit()
    {
        postInit();
        triggerEvent<PostInit>(*this);
    }
    
    void Scene::notifyShutdown()
    {
        shutdown();
        triggerEvent<Shutdown>(*this);
        std::vector<size_t> ids(mEntities.size());
        for(auto & ent : mEntities){
            ids.push_back(ent.first);
        }
        for(auto & id : ids){
            destroyEntity(id);
        }
        mEntities.clear();
        mComponentManager.clear();
        mSystemManager.clear();
        clearQueues();
        clearDelegates();
    }
    
    void Scene::notifyDraw()
    {
        draw();
        triggerEvent<Draw>(*this);
    }
    
    void Scene::notifyReset()
    {
        reset();
        queueEvent<Reset>(*this);
    }

}//end namespace mediasystem
