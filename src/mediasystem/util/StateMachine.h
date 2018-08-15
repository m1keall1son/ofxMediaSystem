//
//  StateMachine.hpp
//  Aerialtronics
//
//  Created by Michael Allison on 8/3/18.
//

#pragma once
#include <functional>
#include <string>
#include <map>
#include <deque>

class StateMachine {
public:
    
    using EnterFn = std::function<void()>;
    using ExitFn = std::function<void()>;
    using TransitioningFn = std::function<void(float)>;
    using TransitionCompleteFn = std::function<void()>;
    using UpdateFn = std::function<void(size_t, float, float)>;
    
    class State {
    public:
        enum class TransitionDirection { IN, OUT };
        
        State(std::string name):mName(std::move(name)){};

        State& transitionDuration(float seconds){ mTransitionInDuration = seconds; mTransitionOutDuration = seconds; return *this; }
        State& transitionInDuration(float seconds){ mTransitionInDuration = seconds; return *this; }
        State& transitionOutDuration(float seconds){ mTransitionOutDuration = seconds; return *this; }
        State& enterFn(EnterFn fn){ mEnter = std::move(fn); return *this; }
        State& exitFn(EnterFn fn){ mExit = std::move(fn); return *this; }
        State& transitioningInFn(TransitioningFn fn){ mTransitioningIn = std::move(fn); return *this; }
        State& transitionInCompleteFn(TransitionCompleteFn fn){ mTransitionInComplete = std::move(fn); return *this; }
        State& transitioningOutFn(TransitioningFn fn){ mTransitioningOut = std::move(fn); return *this; }
        State& transitionOutCompleteFn(TransitionCompleteFn fn){ mTransitionOutComplete = std::move(fn); return *this; }
        State& updateFn(UpdateFn fn){ mUpdate = std::move(fn); return *this; }

        const std::string& getName() const { return mName; }
        State* getPrevState(){ return mPrevState; }
        
        void update(size_t frame, float elapsedTime, float lastFrameTime);
        void enter(float elapsedTime);
        void exit(float elapsedTime);

    private:
    
        std::string mName;
        bool mIsTransitioning{false};
        TransitionDirection mTransitionDirection;
        float mStartTime{0.f};
        float mTransitionInDuration{0.f};
        float mTransitionOutDuration{0.f};
        EnterFn mEnter{nullptr};
        TransitioningFn mTransitioningIn{nullptr};
        TransitionCompleteFn mTransitionInComplete{nullptr};
        ExitFn mExit{nullptr};
        TransitioningFn mTransitioningOut{nullptr};
        TransitionCompleteFn mTransitionOutComplete{nullptr};
        UpdateFn mUpdate{nullptr};
        State* mPrevState{nullptr};
        friend StateMachine;
    };

    void addState(State state);
    void requestState(std::string state, bool force = false);
    void update(size_t frame, float elapsedTime, float lastFrameTime);
    
    const std::string& getCurrentState() const;
    
private:
    
    std::deque<std::string> mRequestQueue;
    State* mCurrentState{nullptr};
    std::map<std::string,State> mStates;
};
