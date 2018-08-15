//
//  StateMachine.cpp
//  Aerialtronics
//
//  Created by Michael Allison on 8/3/18.
//

#include "StateMachine.h"
#include "ofMain.h"

void StateMachine::State::enter(float elapsedTime)
{
    if((mTransitioningIn || mTransitionInComplete) && mTransitionInDuration > 0.f){
        mIsTransitioning = true;
        mTransitionDirection = TransitionDirection::IN;
    }
    mStartTime = elapsedTime;
    if(mEnter)
        mEnter();
}

void StateMachine::State::exit(float elapsedTime)
{
    if((mTransitioningOut || mTransitionOutComplete) && mTransitionOutDuration > 0.f){
        mIsTransitioning = true;
        mTransitionDirection = TransitionDirection::OUT;
    }
    mStartTime = elapsedTime;
    if(mExit)
        mExit();
}

void StateMachine::State::update(size_t frame, float elapsedTime, float lastFrameTime)
{
    if(mIsTransitioning){
        switch(mTransitionDirection){
            case TransitionDirection::IN:{
                auto percent = (elapsedTime - mStartTime) / mTransitionInDuration;
                if(percent >= 1.0f){
                    if(mTransitionInComplete)
                        mTransitionInComplete();
                    mIsTransitioning = false;
                }else{
                    if(mTransitioningIn)
                        mTransitioningIn(percent);
                }
            }break;
            case TransitionDirection::OUT:{
                auto percent = (elapsedTime - mStartTime) / mTransitionOutDuration;
                if(percent >= 1.0f){
                    if(mTransitionOutComplete)
                        mTransitionOutComplete();
                    mIsTransitioning = false;
                }else{
                    if(mTransitioningOut)
                        mTransitioningOut(percent);
                }
            }break;
        }
    }
    if(mUpdate)
        mUpdate(frame,elapsedTime, lastFrameTime);
    if(mPrevState && mPrevState->mIsTransitioning)
        mPrevState->update(frame,elapsedTime, lastFrameTime);
}

void StateMachine::addState(State state)
{
    auto name = state.mName;
    mStates.emplace(name,std::move(state));
}

void StateMachine::requestState(std::string state, bool force)
{
    if(!force){
        if(mCurrentState){
            //already have this state, don't do anything about it.
            if(mCurrentState->getName() == state) return;
        }
    }
    mRequestQueue.emplace_back(std::move(state));
}

const std::string& StateMachine::getCurrentState() const
{
    return mCurrentState->getName();
}

void StateMachine::update(size_t frame, float elapsedTime, float lastFrameTime)
{
    while(!mRequestQueue.empty()){
        auto& requestedState = mRequestQueue.front();
        auto found = mStates.find(requestedState);
        if(found != mStates.end()){
            if(mCurrentState)
                mCurrentState->exit(elapsedTime);
            auto prevState = mCurrentState;
            mCurrentState = &found->second;
            mCurrentState->mPrevState = prevState;
            if(mCurrentState){
               mCurrentState->enter(elapsedTime);
            }
        }else{
            ofLogError("StateMachine") << "No state by name: " << requestedState;
        }
        mRequestQueue.pop_front();
    }
    if(mCurrentState)
        mCurrentState->update(frame, elapsedTime, lastFrameTime);
}
