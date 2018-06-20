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
    
    void Scene::transitionIn()
    {
        if(mTransitionInDuration > 0){
            mTransitionDirection = TRANSITION_IN;
            mIsTransitioning = true;
            mTransitionStart = ofGetElapsedTimef();
            triggerEvent<TransitionInBegin>(*this);
        }
        start();
    }
    
    void Scene::transitionOut()
    {
        if(mTransitionOutDuration > 0){
            mTransitionDirection = TRANSITION_OUT;
            mIsTransitioning = true;
            mTransitionStart = ofGetElapsedTimef();
            triggerEvent<TransitionOutBegin>(*this);
        }else{
            stop();
        }
    }
    
    void Scene::transitionInComplete()
    {
        mIsTransitioning = false;
        triggerEvent<TransitionInComplete>(*this);
    }
    
    void Scene::transitionOutComplete()
    {
        mIsTransitioning = false;
        triggerEvent<TransitionOutComplete>(*this);
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
                transitionUpdate();
                triggerEvent<TransitionUpdate>(*this);
            }
        }
        auto now = ofGetElapsedTimef();
        triggerEvent<Update>(*this,ofGetFrameNum(),now, now - mLastUpdateTime);
        mLastUpdateTime = now;
        //process any events queued by other systems and components, etc.
        processEvents();
    }
    
    void Scene::start()
    {
        mHasStarted = true;
        triggerEvent<Start>(*this);
    }

    void Scene::stop()
    {
        mHasStarted = false;
        mIsTransitioning = false;
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
    
    void Scene::init()
    {
        triggerEvent<Init>(*this);
    }
    
    void Scene::postInit()
    {
        triggerEvent<PostInit>(*this);
    }
    
    void Scene::shutdown()
    {
        triggerEvent<Shutdown>(*this);
        mComponentManager.clear();
        mSystemManager.clear();
        mEntities.clear();
        clearQueues();
        clearDelegates();
    }
    
    void Scene::draw()
    {
        triggerEvent<Draw>(*this);
    }
    
    void Scene::reset()
    {
        queueEvent<Reset>(*this);
    }

}//end namespace mediasystem
