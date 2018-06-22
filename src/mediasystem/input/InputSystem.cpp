//
//  InputSystem.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#include "InputSystem.h"
#include "mediasystem/core/Entity.h"
#include "mediasystem/core/Scene.h"
#include "mediasystem/util/Log.h"
#include "mediasystem/events/SceneEvents.h"
#include "mediasystem/events/GlobalEvents.h"
#include "InputComponent.h"

namespace mediasystem {
    
    InputSystem::InputSystem(Scene& context)
    {
        context.addDelegate<Update>(EventDelegate::create<InputSystem, &InputSystem::onUpdateEvent>(this));
        context.addDelegate<Start>(EventDelegate::create<InputSystem, &InputSystem::onStartEvent>(this));
        context.addDelegate<Stop>(EventDelegate::create<InputSystem, &InputSystem::onStopEvent>(this));
        context.addDelegate<NewComponent<InputComponent>>(EventDelegate::create<InputSystem, &InputSystem::onNewInputComponent>(this));
        
        context.addDelegate<Shutdown>(EventDelegate::create<InputSystem, &InputSystem::onResetEvent>(this));
        addGlobalEventDelegate<SystemReset>(EventDelegate::create<InputSystem, &InputSystem::onResetEvent>(this));
    }
    
    EventStatus InputSystem::onNewInputComponent(const IEventRef& event)
    {
        auto cast = std::static_pointer_cast<NewComponent<InputComponent>>(event);
        if(cast->getComponentType() == &type_id<InputComponent>){
            auto compHandle = cast->getComponentHandle();
            if(auto comp = compHandle.lock()){
                auto z_index = comp->getZIndex();
                auto found = mComponentsByZIndex.find(z_index);
                if(found != mComponentsByZIndex.end()){
                    found->second.emplace_back(compHandle);
                }else{
                    auto l = std::list<InputComponentHandle>();
                    l.emplace_back(compHandle);
                    mComponentsByZIndex.emplace(z_index,std::move(l));
                }
            }
            return EventStatus::SUCCESS;
        }
        MS_LOG_ERROR("Something is wrong, the type_ids should match for in inputsystem on new component!");
        return EventStatus::FAILED;
    }
   
    EventStatus InputSystem::onStartEvent(const IEventRef& event)
    {
        connect();
        return EventStatus::SUCCESS;
    }
    
    EventStatus InputSystem::onStopEvent(const IEventRef& event)
    {
        disconnect();
        return EventStatus::SUCCESS;
    }

    EventStatus InputSystem::onUpdateEvent(const IEventRef& event)
    {
        update();
        return EventStatus::SUCCESS;
    }
    
    EventStatus InputSystem::onResetEvent(const IEventRef& event)
    {
        reset();
        return EventStatus::SUCCESS;
    }
    
    void InputSystem::connect()
    {
        if(mConnected) return;
        ofAddListener(ofEvents().mouseMoved, this, &InputSystem::mouseMove);
        ofAddListener(ofEvents().mouseExited, this, &InputSystem::mouseExit);
        ofAddListener(ofEvents().mouseDragged, this, &InputSystem::mouseDragged);
        ofAddListener(ofEvents().mouseReleased, this, &InputSystem::mouseReleased);
        ofAddListener(ofEvents().mousePressed, this, &InputSystem::mousePressed);
        ofAddListener(ofEvents().mouseScrolled, this, &InputSystem::mouseScrollWheel);
        ofAddListener(ofEvents().keyPressed, this, &InputSystem::keyPressed);
        ofAddListener(ofEvents().keyReleased, this, &InputSystem::keyReleased);
        mConnected = true;
    }
    
    void InputSystem::disconnect()
    {
        if(!mConnected)return;
        ofRemoveListener(ofEvents().mouseMoved, this, &InputSystem::mouseMove);
        ofRemoveListener(ofEvents().mouseExited, this, &InputSystem::mouseExit);
        ofRemoveListener(ofEvents().mouseDragged, this, &InputSystem::mouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &InputSystem::mouseReleased);
        ofRemoveListener(ofEvents().mousePressed, this, &InputSystem::mousePressed);
        ofRemoveListener(ofEvents().mouseScrolled, this, &InputSystem::mouseScrollWheel);
        ofRemoveListener(ofEvents().keyPressed, this, &InputSystem::keyPressed);
        ofRemoveListener(ofEvents().keyReleased, this, &InputSystem::keyReleased);
        mConnected = false;
    }
    
    void InputSystem::update()
    {
        if(!mKeyEvents.empty()){
            for(auto & event : mKeyEvents){
                switch (event.first) {
                    case KEY_PRESSED:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(auto comp = compIt->lock()){
                                    if(comp->isEnabled())
                                        comp->keyPressed(event.second);
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case KEY_RELEASED:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(auto comp = compIt->lock()){
                                    if(comp->isEnabled())
                                        comp->keyReleased(event.second);
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    default:
                        break;
                }
            }
            mKeyEvents.clear();
        }
        
        if(!mMouseEvents.empty()){
            for(auto & event : mMouseEvents){
                bool handled = false;
                switch (event.first) {
                    case MOUSE_MOVE:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            if(handled){
                                break;
                            }
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(handled){
                                    break;
                                }
                                if(auto comp = compIt->lock()){
                                    if(comp->mouseMove(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_EXIT:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            if(handled){
                                break;
                            }
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(handled){
                                    break;
                                }
                                if(auto comp = compIt->lock()){
                                    if(comp->mouseExit(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_DRAGGED:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            if(handled){
                                break;
                            }
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(handled){
                                    break;
                                }
                                if(auto comp = compIt->lock()){
                                    if(comp->mouseDragged(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_PRESSED:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            if(handled){
                                break;
                            }
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(handled){
                                    break;
                                }
                                if(auto comp = compIt->lock()){
                                    if(comp->mousePressed(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_SCROLL:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            if(handled){
                                break;
                            }
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(handled){
                                    break;
                                }
                                if(auto comp = compIt->lock()){
                                    if(comp->mouseScrollWheel(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_RELEASED:
                    {
                        auto layerIt = mComponentsByZIndex.begin();
                        auto layerEnd = mComponentsByZIndex.end();
                        while(layerIt != layerEnd){
                            if(handled){
                                break;
                            }
                            auto compIt = layerIt->second.begin();
                            auto compEnd = layerIt->second.end();
                            while(compIt!=compEnd){
                                if(handled){
                                    break;
                                }
                                if(auto comp = compIt->lock()){
                                    if(comp->mouseReleased(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponentsByZIndex.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                        
                    default:
                        break;
                }
            }
            //todo could pop front like a real queue
            mMouseEvents.clear();
        }
        
        //update components
        {
            auto indexIt = mComponentsByZIndex.begin();
            auto indexEnd = mComponentsByZIndex.end();
            while(indexIt != indexEnd){
                auto layerIt = indexIt->second.begin();
                auto layerEnd = indexIt->second.end();
                while(layerIt != layerEnd){
                    if(auto comp = layerIt->lock()){
                        comp->update();
                        ++layerIt;
                    }else{
                        layerIt = indexIt->second.erase(layerIt);
                    }
                }
                ++indexIt;
            }
        }
    }
    
    void InputSystem::reset()
    {
        mComponentsByZIndex.clear();
        mMouseEvents.clear();
        mKeyEvents.clear();
    }
    
    void InputSystem::mouseMove( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_MOVE, mouse));
    }
    
    void InputSystem::mouseExit( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_EXIT, mouse));
    }
    
    void InputSystem::mousePressed( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_PRESSED, mouse));
    }
    
    void InputSystem::mouseReleased( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_RELEASED, mouse));
    }
    
    void InputSystem::mouseDragged( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_DRAGGED, mouse));
    }
    
    void InputSystem::mouseScrollWheel( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_SCROLL, mouse));
    }
    
    void InputSystem::keyPressed( ofKeyEventArgs& key )
    {
        mKeyEvents.emplace_back(std::make_pair(KEY_PRESSED, key));
    }
    
    void InputSystem::keyReleased( ofKeyEventArgs& key )
    {
        mKeyEvents.emplace_back(std::make_pair(KEY_RELEASED, key));
    }
    
}//end namespace
