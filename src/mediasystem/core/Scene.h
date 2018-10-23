#pragma once
#include <string>
#include <map>
#include "ofMain.h"
#include "mediasystem/events/EventManager.h"
#include "mediasystem/events/SceneEvents.h"
#include "mediasystem/util/StateMachine.h"
#include "mediasystem/core/Handle.h"
#include "mediasystem/memory/Memory.h"

namespace mediasystem {
    
    class Entity;
    using EntityStrongHandle = StrongHandle<Entity>;
    using EntityHandle = Handle<Entity>;
    class Scene;
    
    namespace detail {
        using GenericStrongHandle = StrongHandle<void>;
        using GenericStrongHandleAllocator = Allocator<std::pair<const size_t, GenericStrongHandle>>;
    }
    
    using GenericComponentMap = std::map<size_t, detail::GenericStrongHandle, std::less<size_t>, detail::GenericStrongHandleAllocator>;
    
    //adapter class
    template<typename ComponentType>
    class ComponentMap {
    public:
        
        ComponentMap() = default;
        
        class iterator {
        public:
            StrongHandle<ComponentType> next(){
                if(mIt == mEnd)
                    return nullptr;
                return staticCast<ComponentType>((*mIt++).second);
            }
        private:
            iterator() = default;
            iterator( GenericComponentMap::iterator begin, GenericComponentMap::iterator end ):mIt(begin),mEnd(end){}
            GenericComponentMap::iterator mIt;
            GenericComponentMap::iterator mEnd;
            friend ComponentMap;
        };
        
        iterator iter(){ return mComponents ? iterator(mComponents->begin(), mComponents->end()) : iterator(); }
        size_t size() const { return mComponents ? mComponents->size() : 0; }
        bool empty() const { return mComponents ? mComponents->empty() : true; }
        
    private:
        ComponentMap(GenericComponentMap* components):mComponents(components){}
        GenericComponentMap* mComponents{nullptr};
        friend Scene;
    };
    
    class Scene : public EventManager {
	public:
        
        enum TransitionDir {
            TRANSITION_IN,
            TRANSITION_OUT
        };
        
        virtual ~Scene();
        
        Scene(const std::string& name, AllocationManager&& allocationManager = AllocationManager());
        
        //non copyable
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        
        void notifyInit();
        void notifyPostInit();
        
        void notifyTransitionIn();
        void notifyTransitionOut();
        
        void notifyUpdate(size_t elapsedFrames, float elapsedTime, float prevFrameTime);
        void notifyDraw();
        
        void notifyShutdown();
        void notifyReset();
        
        inline const std::string& getName() const { return mName; }
        
        virtual EntityHandle createEntity();
        virtual bool destroyEntity(size_t id);
        virtual bool destroyEntity(EntityHandle handle);
        virtual EntityHandle getEntity(size_t id);

        template<typename SystemType, typename...Args>
        StrongHandle<SystemType> createSystem(Args&&...args){
            auto shared = makeStrongHandle<SystemType>(std::forward<Args>(args)...);
            auto generic = staticCast<void>(shared);
            auto it = mSystems.emplace(type_id<SystemType>,std::move(generic));
            if(it.second){
                return shared;
            }else{
                MS_LOG_ERROR("Scene: CANNOT CREATE SYSTEM");
                return nullptr;
            }
        }
        
        template<typename SystemType>
        StrongHandle<SystemType> getSystem(){
            auto found = mSystems.find(type_id<SystemType>);
            if(found != mSystems.end()){
                auto shared = staticCast<SystemType>(found->second);
                return std::move(shared);
            }else{
                MS_LOG_ERROR("Scene: DOES NOT HAVE SYSTEM");
                return nullptr;
            }
        }
        
        template<typename SystemType>
        bool destroySystem(){
            auto found = mSystems.find(type_id<SystemType>);
            if(found != mSystems.end()){
                mSystems.erase(found);
                return true;
            }else{
                MS_LOG_ERROR("Scene: DOES NOT HAVE SYSTEM");
                return false;
            }
        }
        
        template<typename ComponentType, typename...Args>
        Handle<ComponentType> createComponent(size_t entity_id, Args&&...args){
            auto shared = allocateStrongHandle<ComponentType>( getAllocator<ComponentType>(), std::forward<Args>(args)...);
            auto generic = staticCast<void>(shared);
            auto found = mComponents.find(type_id<ComponentType>);
            if(found != mComponents.end()){
                auto it = found->second.emplace( entity_id, std::move(generic) );
                if(it.second){
                    queueEvent<NewComponent<ComponentType>>(getEntity(entity_id), shared);
                    return shared;
                }else{
                    MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " COULD NOT CREATE COMPONENT");
                    return Handle<ComponentType>();
                }
            }else{
                auto it = mComponents.emplace(type_id<ComponentType>, GenericComponentMap( getAllocator<std::pair<const size_t, detail::GenericStrongHandle>>() ));
                if(it.second){
                    auto res = it.first->second.emplace(entity_id, std::move(generic));
                    if(res.second){
                        queueEvent<NewComponent<ComponentType>>( getEntity(entity_id), shared);
                        return shared;
                    }else{
                        MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " COULD NOT CREATE COMPONENT");
                        return Handle<ComponentType>();
                    }
                }else{
                    MS_LOG_ERROR("ComponentManager: COULD NOT CREATE GENERIC COMPONENT MAP FOR COMPONENT TYPE");
                    return Handle<ComponentType>();
                }
            }
        }
        
        template<typename ComponentType>
        Handle<ComponentType> getComponent(size_t entity_id){
            auto found = mComponents.find(type_id<ComponentType>);
            if(found != mComponents.end()){
                auto foundComp = found->second.find(entity_id);
                if(foundComp != found->second.end()){
                    return staticCast<ComponentType>(foundComp->second);
                }else{
                    MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                    return Handle<ComponentType>();
                }
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                return Handle<ComponentType>();
            }
        }
        
        Handle<void> getComponent(type_id_t type, size_t entity_id){
            auto found = mComponents.find(type);
            if(found != mComponents.end()){
                auto foundComp = found->second.find(entity_id);
                if(foundComp != found->second.end()){
                    return foundComp->second;
                }else{
                    MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                    return Handle<void>();
                }
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                return Handle<void>();
            }
        }
        
        template<typename ComponentType>
        bool destroyComponent(size_t entity_id){
            auto found = mComponents.find(type_id<ComponentType>);
            if(found != mComponents.end()){
                auto foundComp = found->second.find(entity_id);
                if(foundComp != found->second.end()){
                    found->second.erase(foundComp);
                    return true;
                }else{
                    MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                    return false;
                }
            }else{
                MS_LOG_ERROR("ComponentManager: Entity id: " + std::to_string(entity_id) + " DOES NOT HAVE COMPONENT");
                return false;
            }
        }
        
        bool destroyComponent(type_id_t type, size_t entity_id);
        
        template<typename ComponentType>
        ComponentMap<ComponentType> getComponents(){
            auto found = mComponents.find(type_id<ComponentType>);
            if(found != mComponents.end()){
                return ComponentMap<ComponentType>(&found->second);
            }
            auto it = mComponents.emplace(type_id<ComponentType>, GenericComponentMap( getAllocator<std::pair<const size_t, detail::GenericStrongHandle>>() ));
            if(it.second){
                return ComponentMap<ComponentType>(&it.first->second);
            }else{
                MS_LOG_ERROR("ComponentManager: COULD NOT CREATE GENERIC COMPONENT MAP FOR COMPONENT TYPE");
                return ComponentMap<ComponentType>();
            }
        }
        
        inline bool hasStarted() const { return mHasStarted; }
        
        inline bool isTransitioning() const { return mIsTransitioning; }
        inline TransitionDir getTransitionDirection() const { return mTransitionDirection; }
        float getTransitionDuration(TransitionDir direction) const;
        void setTransitionDuration(TransitionDir direction, float duration);
        float getPercentTransitionComplete() const;
        
        void requestState(std::string state);
        void addState(StateMachine::State&& state);
        void addChildState(std::string parent, StateMachine::State&& state);
        
        template<typename T>
        Allocator<T> getAllocator(const AllocationPolicyFormat& fmt = AllocationPolicyFormat()){
            return Allocator<T>(&mAllocationManager, fmt);
        }
        
	protected:
        
        virtual void init(){}
        virtual void postInit(){}
        
        virtual void transitionIn(){}
        virtual void transitionUpdate(){}
        virtual void transitionOut(){}
        
        virtual void update(size_t elapsedFrames, float elapsedTime, float prevFrameTime){}
        virtual void draw(){}
        
        virtual void shutdown(){}
        virtual void reset(){}
        
        virtual void start(){}
        virtual void stop(){}
        
        virtual void transitionInComplete(){}
        virtual void transitionOutComplete(){}
        
        inline const std::string& getPreviousSceneName() const { return mPreviousScene; }
        
        void clearSystems();
        
		std::string	mName;
        AllocationManager mAllocationManager;
        std::map<size_t, EntityStrongHandle, std::less<size_t>, Allocator<std::pair<const size_t, EntityStrongHandle>>> mEntities{ Allocator<std::pair<const size_t, EntityStrongHandle>>(&mAllocationManager) };

	private:
        
        void clearComponents();
        
        void collectEntities();
        
        void notifyStart();
        void notifyStop();
        
        void notifyTransitionInComplete();
        void notifyTransitionOutComplete();
        
        TransitionDir mTransitionDirection{TRANSITION_IN};
        bool mIsTransitioning{false};
        float mTransitionInDuration{0.f};
        float mTransitionOutDuration{0.f};
        float mTransitionStart{0.f};
        
        bool mHasStarted{false};
        std::map<type_id_t, GenericComponentMap> mComponents;
        std::map<type_id_t, StrongHandle<void>> mSystems;
        std::deque<size_t> mDestroyedEntities;
        std::string mPreviousScene;
        StateMachine mSequence;
        friend class SceneManager;
	};
    
}//end namespace mediasystem
