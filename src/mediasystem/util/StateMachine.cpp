//
//  StateMachine.c
//  Aerialtronics
//
//  Created by Michael Allison on 8/16/18.
//

#include "StateMachine.h"
#include "ofMain.h"

namespace mediasystem {

    StateMachine::Transition::Transition(float dur):
        duration(dur)
    {}

    void StateMachine::addState(State state)
    {
        auto name = state.mName;
        state.mParent = &mRoot;
        state.mDepth = mRoot.mDepth + 1;
        auto it = mStates.emplace(std::move(name),std::move(state));
    }

    void StateMachine::addChildState(std::string parent, State state)
    {
        auto found = mStates.find(parent);
        if(found != mStates.end()){
            auto name = state.mName;
            state.mParent = &found->second;
            state.mDepth = found->second.mDepth + 1;
            mStates.emplace(std::move(name),std::move(state));
        }else{
            ofLogError("StateMachine") << "cant find parent state: \'" << parent << "\'. adding to root.";
            addState(std::move(state));
        }
    }

    void StateMachine::removeState(std::string state)
    {
        auto found = mStates.find(state);
        if(found != mStates.end()){
            for(auto & state : mStates){
                if(state.second.mParent == &found->second){
                    state.second.mParent = found->second.mParent;
                    state.second.mDepth = found->second.mDepth;
                }
            }
            mStates.erase(found);
        }else{
            ofLogError("StateMachine") << "cant find state: " << state;
        }
    }

    void StateMachine::changeParentTo(std::string state, std::string newParent)
    {
        auto foundState = mStates.find(state);
        if(foundState != mStates.end()){
            auto foundParent = mStates.find(newParent);
            if(foundParent != mStates.end()){
                foundState->second.mParent = &foundParent->second;
                foundState->second.mDepth = foundParent->second.mDepth;
            }else{
                ofLogError("StateMachine") << "cant find new parent: " << newParent;
            }
        }else{
            ofLogError("StateMachine") << "cant find state: " << state;
        }
    }

    void StateMachine::requestState(std::string state, bool force)
    {
        if(force){
            mRequestQueue.clear();
        }
        Request req;
        req.state = std::move(state);
        mRequestQueue.emplace_back(std::move(req));
    }

    void StateMachine::requestState(std::string state, Transition transition, bool force)
    {
        if(!force){
            mRequestQueue.clear();
        }
        Request req;
        req.state = std::move(state);
        req.transition = std::move(transition);
        mRequestQueue.emplace_back(std::move(req));
    }

    void StateMachine::update(size_t frame, float elapsedTime, float lastFrameTime)
    {
        if(mIsTransitioning){
            
            auto percent = (elapsedTime - mTransition.startTime) / mTransition.duration;
            
            switch (mTransition.type) {
                case Transition::Type::TRANSITION_IN_ONLY:
                {
                    if(percent >= 1.0f){
                        for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                            auto state = (*i);
                            if(state->mTransitionInComplete)
                                state->mTransitionInComplete();
                        }
                        std::swap(mCurrent,mNext);
                        mNext.clear();
                        mIsTransitioning = false;
                    }else{
                        for(auto state : mNext){
                            if(state->mTransitionUpdate){
                                if(mTransition.easing)
                                    state->mTransitionUpdate(TransitionDirection::IN, mTransition.easing(percent));
                                else
                                    state->mTransitionUpdate(TransitionDirection::IN,percent);
                            }
                        }
                    }
                    
                }break;
                case Transition::Type::TRANSITION_OUT_ONLY:
                {
                    if(percent >= 1.0f){
                        for(auto state : mCurrent){
                            if(state->mTransitionOutComplete)
                                state->mTransitionOutComplete();
                        }
                        for(auto state : mCurrent){
                            if(state->mExit)
                                state->mExit();
                        }
                        std::swap(mCurrent,mNext);
                        mNext.clear();
                        mIsTransitioning = false;
                    }else{
                        for(auto state : mCurrent){
                            if(state->mTransitionUpdate){
                                if(mTransition.easing)
                                    state->mTransitionUpdate(TransitionDirection::OUT, mTransition.easing(percent));
                                else
                                    state->mTransitionUpdate(TransitionDirection::OUT,percent);
                            }
                        }
                    }
                }break;
                case Transition::Type::STAGGERED_TRANSITION:
                {
                    if(percent >= 1.0f){
                        //done
                        for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                            auto state = (*i);
                            if(state->mTransitionInComplete)
                                state->mTransitionInComplete();
                        }
                        std::swap(mCurrent,mNext);
                        mNext.clear();
                        mIsTransitioning = false;
                    }
                    
                    //do one then the other
                    if(percent >= 0.5f){
                        //update the second half
                        if(!mStaggeredTransitionHalfComplete){
                            for(auto state : mCurrent){
                                if(state->mTransitionOutComplete)
                                    state->mTransitionOutComplete();
                            }
                            for(auto state : mCurrent){
                                if(state->mExit)
                                    state->mExit();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mEnter)
                                    state->mEnter();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mTransitionIn)
                                    state->mTransitionIn();
                            }
                            
                            mStaggeredTransitionHalfComplete = true;
                        }
                        for(auto state : mNext){
                            if(state->mTransitionUpdate){
                                if(mTransition.easing)
                                    state->mTransitionUpdate(TransitionDirection::IN, mTransition.easing(percent*2.f - 1.f));
                                else
                                    state->mTransitionUpdate(TransitionDirection::IN,percent*2.f - 1.f);
                            }
                        }
                    }else{
                        //update the first half
                        for(auto state : mCurrent){
                            if(state->mTransitionUpdate){
                                if(mTransition.easing)
                                    state->mTransitionUpdate(TransitionDirection::OUT, mTransition.easing(percent*2.f));
                                else
                                    state->mTransitionUpdate(TransitionDirection::OUT,percent*2.f);
                            }
                        }
                    }
                }break;
                case Transition::Type::SIMULTANEOUS_TRANSITION:
                {
                    if(percent >= 1.0f){
                        for(auto state : mCurrent){
                            if(state->mTransitionOutComplete)
                                state->mTransitionOutComplete();
                        }
                        
                        for(auto state : mCurrent){
                            if(state->mExit)
                                state->mExit();
                        }
                        
                        for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                            auto state = (*i);
                            if(state->mTransitionInComplete)
                                state->mTransitionInComplete();
                        }
                        
                        std::swap(mCurrent,mNext);
                        mNext.clear();
                        mIsTransitioning = false;
                    }else{
                        for(auto state : mCurrent){
                            if(state->mTransitionUpdate){
                                if(mTransition.easing)
                                    state->mTransitionUpdate(TransitionDirection::OUT, mTransition.easing(percent));
                                else
                                    state->mTransitionUpdate(TransitionDirection::OUT,percent);
                            }
                        }
                        for(auto state : mNext){
                            if(state->mTransitionUpdate){
                                if(mTransition.easing)
                                    state->mTransitionUpdate(TransitionDirection::IN, mTransition.easing(percent));
                                else
                                    state->mTransitionUpdate(TransitionDirection::IN,percent);
                            }
                        }
                    }
                }break;
                default: break;
            }
            
            if(!mCurrent.empty()){
                for(auto state : mCurrent){
                    if(state->mUpdate)
                        state->mUpdate(frame, elapsedTime, lastFrameTime);
                }
            }
            
            if(!mNext.empty()){
                for(auto state : mNext){
                    if(state->mUpdate)
                        state->mUpdate(frame, elapsedTime, lastFrameTime);
                }
            }
            
        }else{
            bool processRequests = !mRequestQueue.empty();
            while(processRequests){
                auto req = mRequestQueue.front();
                mRequestQueue.pop_front();
                auto found = mStates.find(req.state);
                if(found != mStates.end()){
                    
                    mNext.clear();
                    State* end = &found->second;
                    State* start = nullptr;
                    
                    if(!mCurrent.empty()){
                        start = mCurrent.front();
                        mCurrent.clear();
                    }
                    
                    findCommonParent(&mRoot, start, end, mCurrent, mNext);
                    
                    switch(req.transition.type){
                        case Transition::Type::NONE:
                        {
                            for(auto state : mCurrent){
                                if(state->mExit)
                                    state->mExit();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mEnter)
                                    state->mEnter();
                            }
                            
                            std::swap(mCurrent,mNext);
                            mNext.clear();
                        }break;
                        case Transition::Type::TRANSITION_IN_ONLY:{
                            
                            mTransition = req.transition;
                            mTransition.startTime = elapsedTime;
                            
                            for(auto state : mCurrent){
                                if(state->mExit)
                                    state->mExit();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mEnter)
                                    state->mEnter();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mTransitionIn)
                                    state->mTransitionIn();
                            }
                            
                            mIsTransitioning = true;
                            processRequests = false;
                            
                        }break;
                        case Transition::Type::TRANSITION_OUT_ONLY:{
                            
                            mTransition = req.transition;
                            mTransition.startTime = elapsedTime;
                            
                            for(auto state : mCurrent){
                                if(state->mTransitionOut)
                                    state->mTransitionOut();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mEnter)
                                    state->mEnter();
                            }
                            
                            mIsTransitioning = true;
                            processRequests = false;
                            
                        }break;
                        case Transition::Type::STAGGERED_TRANSITION:{
                            
                            mStaggeredTransitionHalfComplete = false;
                            mTransition = req.transition;
                            mTransition.startTime = elapsedTime;
                            
                            for(auto state : mCurrent){
                                if(state->mTransitionOut)
                                    state->mTransitionOut();
                            }
                            
                            mIsTransitioning = true;
                            processRequests = false;
                            
                        }break;
                        case Transition::Type::SIMULTANEOUS_TRANSITION:
                        {
                            mTransition = req.transition;
                            mTransition.startTime = elapsedTime;
                            
                            for(auto state : mCurrent){
                                if(state->mTransitionOut)
                                    state->mTransitionOut();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mEnter)
                                    state->mEnter();
                            }
                            
                            for (auto i = mNext.rbegin(); i != mNext.rend(); ++i){
                                auto state = (*i);
                                if(state->mTransitionIn)
                                    state->mTransitionIn();
                            }
                            
                            mIsTransitioning = true;
                            processRequests = false;
                        }break;
                    }
                }else{
                    ofLogError("StateMachine") << "No state by name: " << req.state;
                }
                processRequests = !mRequestQueue.empty();
            }
            
            if(!mCurrent.empty()){
                for(auto state : mCurrent){
                    if(state->mUpdate)
                        state->mUpdate(frame, elapsedTime, lastFrameTime);
                }
            }
        }
    }

    const std::string& StateMachine::getCurrentState() const { return mCurrent.front()->getName(); }

    void StateMachine::findCommonParent( State* root, State* start, State* end, std::list<State*>& curStack, std::list<State*>& nextStack)
    {
        if(start == end){
            //edge case of repeating a state
            curStack.push_back(start);
            nextStack.push_back(end);
            return;
        }else if(!start){
            //edge case for no state to begin with, just traverse the enter tree
            while(end != root){
                nextStack.push_back(end);
                end = end->mParent;
            }
        }else{
            
            curStack.clear();
            nextStack.clear();
            
            //traverse trees up to root
            while(start != nullptr){
                curStack.push_back(start);
                start = start->mParent;
            }
            while(end != nullptr){
                nextStack.push_back(end);
                end = end->mParent;
            }
            
            //find the common node
            std::list<State*>::iterator exitCommon;
            std::list<State*>::iterator enterCommon;
            auto curIt = curStack.begin();
            auto curEnd = curStack.end();
            bool search = true;
            while(curIt != curEnd && search){
                auto nextIt = nextStack.begin();
                auto nextEnd = nextStack.end();
                while(nextIt != nextEnd && search){
                    if(*curIt == *nextIt){
                        exitCommon = curIt;
                        enterCommon = nextIt;
                        search = false;
                    }
                    ++nextIt;
                }
                ++curIt;
            }
            //erase from the common node to the root
            curStack.erase(exitCommon, curStack.end());
            nextStack.erase(enterCommon, nextStack.end());
        }
    }

}//end namespace media system
