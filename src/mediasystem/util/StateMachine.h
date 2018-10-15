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
#include <list>
#include <vector>

namespace mediasystem {

    class StateMachine {
    public:
        
        enum class TransitionDirection { IN, OUT };

        using EnterFn = std::function<void()>;
        using ExitFn = std::function<void()>;
        using UpdateFn = std::function<void(size_t, float, float)>;
        using TransitioningFn = std::function<void(TransitionDirection, float)>;
        using TransitionFn = std::function<void()>;
      
        class State {
        public:

            State(std::string name):mName(std::move(name)){};
            
            State(const State&) = delete;
            State& operator=(const State&) = delete;
            
            State(State&&) = default;
            State& operator=(State&&) = default;

            State& enterFn(EnterFn fn){ mEnter = std::move(fn); return *this; }
            State& exitFn(EnterFn fn){ mExit = std::move(fn); return *this; }
            State& updateFn(UpdateFn fn){ mUpdate = std::move(fn); return *this; }

            State& transitionInStartFn(TransitionFn fn){ mTransitionIn = std::move(fn); return *this; }
            State& transitionInCompleteFn(TransitionFn fn){ mTransitionInComplete = std::move(fn); return *this; }
            State& transitionOutStartFn(TransitionFn fn){ mTransitionOut = std::move(fn); return *this; }
            State& transitionOutCompleteFn(TransitionFn fn){ mTransitionOutComplete = std::move(fn); return *this; }
            State& transitionUpdateFn(TransitioningFn fn){ mTransitionUpdate = std::move(fn); return *this; }
            
            const std::string& getName() const { return mName; }

        private:
            
            std::string mName;
            EnterFn mEnter{nullptr};
            ExitFn mExit{nullptr};
            UpdateFn mUpdate{nullptr};
            State* mParent{nullptr};
            size_t mDepth{0};
            
            TransitionFn mTransitionIn{nullptr};
            TransitionFn mTransitionInComplete{nullptr};
            TransitionFn mTransitionOut{nullptr};
            TransitionFn mTransitionOutComplete{nullptr};
            TransitioningFn mTransitionUpdate;
            
            friend StateMachine;
        };
        
        struct Transition {
            enum class Type { NONE, TRANSITION_IN_ONLY, TRANSITION_OUT_ONLY, STAGGERED_TRANSITION, SIMULTANEOUS_TRANSITION };
            Transition(float duration = 0.f);
            
            Transition& disable(){ type = Type::NONE; return *this; };
            Transition& simultaneous(){ type = Type::SIMULTANEOUS_TRANSITION; return *this; };
            Transition& staggered(){ type = Type::STAGGERED_TRANSITION; return *this; };
            Transition& inOnly(){ type = Type::TRANSITION_IN_ONLY; return *this; };
            Transition& outOnly(){ type = Type::TRANSITION_OUT_ONLY; return *this; };
            Transition& easeFn(std::function<float(float)> easeFn){ easing = std::move(easeFn); return *this; };

            Type type{Type::NONE};
            float duration{0.f};
            std::function<float(float)> easing;
        private:
            float startTime{0.f};
            friend StateMachine;
        };
        
        void addState(State&& state);
        void addChildState(std::string parent, State&& state);
        void removeState(std::string state);
        void changeParentTo(std::string state, std::string newParent);
        void requestState(std::string state, bool force = false);
        void requestState(std::string state, Transition transition, bool force = false);

        void update(size_t frame, float elapsedTime, float lastFrameTime);

        const std::string& getCurrentState() const;

    private:
        
        static void findCommonParent( State* root, State* start, State* end, std::list<State*>& curStack, std::list<State*>& nextStack );
        
        struct Request {
            Request() = default;
            std::string state;
            Transition transition;
        };
        
        std::list<Request> mRequestQueue;
        std::list<State*> mCurrent;
        std::list<State*> mNext;
        std::map<std::string,State> mStates;
        State mRoot{"root"};
        Transition mTransition;
        bool mIsTransitioning{false};
        bool mStaggeredTransitionHalfComplete{false};
        
    };
    
}//end namespace mediasystem
