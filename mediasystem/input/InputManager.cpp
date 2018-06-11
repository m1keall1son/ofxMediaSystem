//
//  InputManager.cpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/17/18.
//

#include "InputManager.h"

namespace mediasystem {
    
    InputManager::InputManager()
    {}
    
    void InputManager::connect()
    {
        if(mConnected) return;
        ofAddListener(ofEvents().mouseMoved, this, &InputManager::mouseMove);
        ofAddListener(ofEvents().mouseExited, this, &InputManager::mouseExit);
        ofAddListener(ofEvents().mouseDragged, this, &InputManager::mouseDragged);
        ofAddListener(ofEvents().mouseReleased, this, &InputManager::mouseReleased);
        ofAddListener(ofEvents().mousePressed, this, &InputManager::mousePressed);
        ofAddListener(ofEvents().mouseScrolled, this, &InputManager::mouseScrollWheel);
        ofAddListener(ofEvents().keyPressed, this, &InputManager::keyPressed);
        ofAddListener(ofEvents().keyReleased, this, &InputManager::keyReleased);
        mConnected = true;
    }
    
    void InputManager::disconnect()
    {
        if(!mConnected)return;
        ofRemoveListener(ofEvents().mouseMoved, this, &InputManager::mouseMove);
        ofRemoveListener(ofEvents().mouseExited, this, &InputManager::mouseExit);
        ofRemoveListener(ofEvents().mouseDragged, this, &InputManager::mouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &InputManager::mouseReleased);
        ofRemoveListener(ofEvents().mousePressed, this, &InputManager::mousePressed);
        ofRemoveListener(ofEvents().mouseScrolled, this, &InputManager::mouseScrollWheel);
        ofRemoveListener(ofEvents().keyPressed, this, &InputManager::keyPressed);
        ofRemoveListener(ofEvents().keyReleased, this, &InputManager::keyReleased);
        mConnected = false;
    }
    
    void InputManager::update()
    {
        if(!mKeyEvents.empty()){
            for(auto & event : mKeyEvents){
                switch (event.first) {
                    case KEY_PRESSED:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                layerIt = mComponents.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case KEY_RELEASED:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                layerIt = mComponents.erase(layerIt);
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
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                    if(comp->isEnabled() && comp->mouseMove(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponents.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_EXIT:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                    if(comp->isEnabled() && comp->mouseExit(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponents.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_DRAGGED:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                    if(comp->isEnabled() && comp->mouseDragged(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponents.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_PRESSED:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                    if(comp->isEnabled() && comp->mousePressed(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponents.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_SCROLL:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                    if(comp->isEnabled() && comp->mouseScrollWheel(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponents.erase(layerIt);
                            else
                                ++layerIt;
                        }
                    }break;
                    case MOUSE_RELEASED:
                    {
                        auto layerIt = mComponents.begin();
                        auto layerEnd = mComponents.end();
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
                                    if(comp->isEnabled() && comp->mouseReleased(event.second))
                                    {
                                        handled = true;
                                    }
                                    ++compIt;
                                }else{
                                    compIt = layerIt->second.erase(compIt);
                                }
                            }
                            if(layerIt->second.empty())
                                layerIt = mComponents.erase(layerIt);
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
        
    }
    
    void InputManager::mouseMove( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_MOVE, mouse));
    }
    
    void InputManager::mouseExit( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_EXIT, mouse));
    }
    
    void InputManager::mousePressed( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_PRESSED, mouse));
    }
    
    void InputManager::mouseReleased( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_RELEASED, mouse));
    }
    
    void InputManager::mouseDragged( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_DRAGGED, mouse));
    }
    
    void InputManager::mouseScrollWheel( ofMouseEventArgs& mouse )
    {
        mMouseEvents.emplace_back(std::make_pair(MOUSE_SCROLL, mouse));
    }
    
    void InputManager::keyPressed( ofKeyEventArgs& key )
    {
        mKeyEvents.emplace_back(std::make_pair(KEY_PRESSED, key));
    }
    
    void InputManager::keyReleased( ofKeyEventArgs& key )
    {
        mKeyEvents.emplace_back(std::make_pair(KEY_RELEASED, key));
    }
    
    void InputManager::registerInputComponent(const InputComponentHandle& handle)
    {
        if(auto comp = handle.lock()){
            auto found = mComponents.find(comp->getZIndex());
            if(found != mComponents.end()){
                found->second.push_back(handle);
            }else{
                mComponents.insert({comp->getZIndex(), {handle} });
            }
        }
    }
    
    void InputManager::registerInputComponent(InputComponentHandle&& handle)
    {
        if(auto comp = handle.lock()){
            auto found = mComponents.find(comp->getZIndex());
            if(found != mComponents.end()){
                found->second.emplace_back(std::move(handle));
            }else{
                auto l = std::list<InputComponentHandle>();
                l.emplace_back(std::move(handle));
                mComponents.emplace(comp->getZIndex(),std::move(l));
            }
        }
    }
    
}//end namespace
