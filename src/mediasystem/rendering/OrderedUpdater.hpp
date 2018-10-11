//
//  OrderedUpdater.hpp
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
    class Updateable : public T {
    public:
        
        template<typename...Args>
        Updateable(Args&&...args):
        T(std::forward<Args>(args)...)
        {}
    
        //orderable concept
        void setUpdateOrder(float order) const { mOrder = order; }
        float getUpdateOrder() const { return mOrder; }
        float setUpdateEnabled(bool set) const { return mEnabled = set; }
        bool isUpdateEnabled()const{ return mEnabled; }
        
        //updateable concept
        void update(){ T::update(); }
        
    private:
        float mOrder{0.f};
        bool mEnabled{true};
    };
    
    template<typename T>
    using UpdateableHandle = Handle<Updateable<T>>;
    
    template<typename T>
    using UpdateableHandleList = std::list<UpdateableHandle<T>,DynamicAllocator<UpdateableHandle<T>>>;
    
    template<typename...UpdateableTypes>
    class OrderedUpdater {
        
        using OrderedList = std::tuple<UpdateableHandleList<UpdateableTypes>...>;
        using OderedListsMap = std::map<float, OrderedList>;
        
    public:
        
        OrderedUpdater(Scene& scene):
        mScene(scene)
        {
            mScene.addDelegate<Update>(EventDelegate::create<OrderedUpdater,&OrderedUpdater::onUpdate>(this));
            int l[] = {(addNewComponentDelegate<UpdateableTypes>(),0)...};
            UNUSED_VARIABLE(l);
        }
        
        ~OrderedUpdater(){
            mScene.removeDelegate<Update>(EventDelegate::create<OrderedUpdater,&OrderedUpdater::onUpdate>(this));
            int l[] = {(removeNewComponentDelegate<UpdateableTypes>(),0)...};
            UNUSED_VARIABLE(l);
        }
    
        void update(){
            for ( auto & order : mOrderedLists ) {
                for_each_in_tuple(order.second,OrderChecker<UpdateableTypes...>(*this,order.first));
            }
            for ( auto & order : mOrderedLists ) {
                for_each_in_tuple(order.second,OrderedUpdate<UpdateableTypes...>(*this));
            }
        }
        
    private:
        
        template<typename...Args>
        struct OrderChecker {
            OrderChecker(OrderedUpdater<Args...>& renderer, float order):mUpdater(renderer), mOrder(order){}
            template<typename T>
            void operator ()(T&& t){
                mUpdater.checkOrder(mOrder, t);
            }
            OrderedUpdater<Args...>& mUpdater;
            float mOrder{0.f};
        };
        
        template<typename...Args>
        struct OrderedUpdate {
            OrderedUpdate(OrderedUpdater<Args...>& renderer):mUpdater(renderer){}
            template<typename T>
            void operator ()(T&& t){
                mUpdater.updateOrderedList(t);
            }
            OrderedUpdater<Args...>& mUpdater;
        };
        
        template<typename T>
        void addNewComponentDelegate(){
            mScene.addDelegate<NewComponent<Updateable<T>>>(EventDelegate::create<OrderedUpdater, &OrderedUpdater::onNewOrderedComponent<T>>(this));
        }
        
        template<typename T>
        void removeNewComponentDelegate(){
            mScene.removeDelegate<NewComponent<Updateable<T>>>(EventDelegate::create<OrderedUpdater, &OrderedUpdater::onNewOrderedComponent<T>>(this));
        }
        
        template<typename T>
        void insertIntoOrderedList(float order, UpdateableHandle<T> handle){
            auto found = mOrderedLists.find(order);
            if(found != mOrderedLists.end()){
                auto& list = get_element_by_type<UpdateableHandleList<T>>(found->second);
                list.emplace_back(std::move(handle));
            }else{
                OrderedList l;
                auto& list = get_element_by_type<UpdateableHandleList<T>>(l);
                list.emplace_back(std::move(handle));
                mOrderedLists.emplace(order, std::move(l));
            }
        }
        
        template<typename T>
        void checkOrder(float order, UpdateableHandleList<T>& handles){
            auto it = handles.begin();
            auto end = handles.end();
            while( it != end ){
                if(auto component = it->lock()){
                    if(component->getLayer() != order){
                        it = handles.erase(it);
                        insertIntoOrderedList<T>(component->getUpdateOrder(), std::move(component));
                    }else{
                        ++it;
                    }
                }else{
                    it = handles.erase(it);
                }
            }
        }
        
        template<typename T>
        void updateOrderedList(UpdateableHandleList<T>& handles){
            auto it = handles.begin();
            auto end = handles.end();
            while (it!=end) {
                if (auto component = (*it).lock()) {
                    if (component->isUpdateEnabled()) {
                        component->update();
                    }
                    ++it;
                }
                else {
                    it = handles.erase(it);
                }
            }
        }
        
        template<typename T>
        EventStatus onNewOrderedComponent( const IEventRef& event ){
            auto cast = std::static_pointer_cast<NewComponent<Updateable<T>>>(event);
            if(cast->getComponentType() == &type_id<Updateable<T>>){
                auto compHandle = cast->getComponentHandle();
                if(auto comp = compHandle.lock()){
                    auto order = comp->getUpdateOrder();
                    insertIntoOrderedList<T>(order, std::move(compHandle));
                }
                return EventStatus::SUCCESS;
            }
            MS_LOG_ERROR("Something is wrong, the type_ids should match on new component event!");
            return EventStatus::FAILED;
        }
        
        EventStatus onUpdate( const IEventRef& event ){
            update();
            return EventStatus::SUCCESS;
        }
        
        Scene& mScene;
        OderedListsMap mOrderedLists;
    };
}//end namespace

