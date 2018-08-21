//
//  Playable.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/4/18.
//

#pragma once

#include "mediasystem/core/Scene.h"
#include "mediasystem/util/Playable.hpp"
#include "Easing.hpp"
#include "ofMain.h"

namespace mediasystem {

    template <typename T>
    struct Lerp {
        T operator()(const T& start, const T& end, const float& t){
            return (1.f-t)*start + t*end;
        }
    };

    template <typename T>
    struct Slerp {
        T operator()(const T& start, const T& end, const float& t){
            return glm::slerp(start, end, t);
        }
    };

    struct RectLerp {
        ofRectangle operator()(const ofRectangle& start, const ofRectangle& end, const float& t){
            return ofRectangle( Lerp<glm::vec3>()(start.getTopLeft(), end.getTopLeft(), t), Lerp<glm::vec3>()(start.getBottomRight(), end.getBottomRight(), t));
        }
    };
    
    struct FloatColorLerp {
        ofFloatColor operator()(const ofFloatColor& start, const ofFloatColor& end, const float& t){
            return ofFloatColor( Lerp<float>()(start.r, end.r, t), Lerp<float>()(start.g, end.g, t), Lerp<float>()(start.b, end.b, t), Lerp<float>()(start.a, end.a, t));
        }
    };

    using Animator = Playable<float>;
    using EaseFn = std::function<float(float)>;

    template<typename T, typename Tween = Lerp<T>>
    class Animatable : public Animator {
    public:
        
        using lerp_type = Tween;
        using Options = Animator::Options;

        Animatable(T* target, T start, T end, float duration, EaseFn easing = nullptr, Options opts = Options()):
            Playable(duration, std::move(opts)),
            mStart(start),
            mEnd(end),
            mCurrent(start),
            mTarget(target),
            mEaseFn(std::move(easing))
        {
            auto update = opts.getUpdateFn();
            setUpdateFn(update);
            if(opts.isReversed()){
                mCurrent = mEnd;
            }
        }

        Animatable(T start, T end, float duration, EaseFn easing = nullptr, Options opts = Options()):
            Playable(duration, std::move(opts)),
            mStart(start),
            mEnd(end),
            mCurrent(start),
            mEaseFn(std::move(easing))
        {
            auto update = opts.getUpdateFn();
            setUpdateFn(update);
            if(opts.isReversed()){
                mCurrent = mEnd;
            }
        }
        
        Animatable(T start, float duration, EaseFn easing = nullptr, Options opts = Options()):
        Playable(duration, std::move(opts)),
        mStart(start),
        mEnd(start),
        mCurrent(start),
        mEaseFn(std::move(easing))
        {
            auto update = opts.getUpdateFn();
            setUpdateFn(update);
            if(opts.isReversed()){
                mCurrent = mEnd;
            }
        }
        
        void setUpdateFn(std::function<void()> update) override {
            mOnUpdateFn = [&,update](){
                auto percent = getPercentComplete();
                if(mEaseFn)
                    percent = mEaseFn(percent);
                mCurrent = Tween()(mStart, mEnd, percent);
                if(mTarget)
                    *mTarget = mCurrent;
                if(update)
                    update();
            };
        }
        
        void animateTo(T end){ mStart = mCurrent; mEnd = std::move(end); play(); }
        void animateFromTo(T start, T end){ mStart = std::move(start); mEnd = std::move(end); play(); }
        void setEasingFn(EaseFn easing){ mEaseFn = std::move(easing); }
        void setStartValue(T start){ mStart = std::move(start); }
        void setEndValue(T end){ mEnd = std::move(end); }
        void setUpdateTarget(T* target){ mTarget = target; }
        void clearUpdateTarget(){ mTarget = nullptr; }
        T value() { return mCurrent; }
        const T& getStartValue() const { return mStart; }
        const T& getEndValue() const { return mEnd; }

    private:
        T mStart{0};
        T mEnd{0};
        T mCurrent{0};
        T* mTarget{nullptr};
        EaseFn mEaseFn{nullptr};
    };

    using AnimatedFloat = Animatable<float>;
    using AnimatedVec2 = Animatable<glm::vec2>;
    using AnimatedVec3 = Animatable<glm::vec3>;
    using AnimatedVec4 = Animatable<glm::vec4>;
    using AnimatedQuat = Animatable<glm::quat,Slerp<glm::quat>>;
    using AnimatedRect = Animatable<ofRectangle,RectLerp>;
    using AnimatedFloatColor = Animatable<ofFloatColor,FloatColorLerp>;

    template<typename...AnimationTypes>
    class AnimationManager;

    using AnimationSystem = AnimationManager<AnimatedFloat,AnimatedVec2,AnimatedVec3,AnimatedVec4,AnimatedQuat,AnimatedRect, AnimatedFloatColor>;

    template<typename...AnimationTypes>
    class AnimationManager {
    public:
        
        AnimationManager(Scene& scene):
            mScene(scene)
        {
            bool a[] = {addDelegates<AnimationTypes>()...};
            (void)a;
            mScene.addDelegate<Update>(EventDelegate::create<AnimationManager,&AnimationManager::onUpdate>(this));
        }
        
        ~AnimationManager()
        {
            bool a[] = {removeDelegates<AnimationTypes>()...};
            (void)a;
            mScene.removeDelegate<Update>(EventDelegate::create<AnimationManager,&AnimationManager::onUpdate>(this));
        }
        
        std::shared_ptr<Animatable<float>> createAnimation(std::string name, float start, float end, float duration, EaseFn easing = nullptr, Animatable<float>::Options opts = Animatable<float>::Options()){
            std::shared_ptr<Animatable<float>> ret;
            ret.reset(new Animatable<float>(start, end, duration, std::move(easing), std::move(opts)));
            auto it = mAnimations.emplace(std::move(name),std::weak_ptr<Animatable<float>>(ret));
            if(it.first){
                return ret;
            }else{
                return it.second;
            }
        }
        
    private:
        
        template<typename T>
        bool addDelegates(){
            mScene.addDelegate<NewComponent<T>>(EventDelegate::create<AnimationManager,&AnimationManager::onNewAnimation<T>>(this));
            return true;
        }
        
        template<typename T>
        bool removeDelegates(){
            mScene.removeDelegate<NewComponent<T>>(EventDelegate::create<AnimationManager,&AnimationManager::onNewAnimation<T>>(this));
            return true;
        }
        
        template<typename T>
        EventStatus onNewAnimation(const IEventRef& event){
            static_assert(std::is_base_of<Animator,T>::value, "T must derive from Animator, ie be some Animatable<type>");
            auto newCompEvent = std::static_pointer_cast<NewComponent<T>>(event);
            auto handle = newCompEvent->getComponentHandle();
            auto anim = std::static_pointer_cast<Animator>(handle.lock());
            auto weakGeneric = std::weak_ptr<Animator>(anim);
            mAnimationComponents.push_back(weakGeneric);
            return EventStatus::SUCCESS;
        }
        
        //this should be a synced event maybe
        EventStatus onUpdate(const IEventRef& event){
            auto update = std::static_pointer_cast<Update>(event);
            
            //step all animations, remove if fails to lock
            auto animIt = mAnimations.begin();
            auto animEnd = mAnimations.end();
            while(animIt != animEnd){
                if(auto animator = animIt->second.lock()){
                    animator->step(update->getLastFrameTime());
                    ++animIt;
                }else{
                    animIt = mAnimations.erase(animIt);
                }
            }
            
            auto compIt = mAnimationComponents.begin();
            auto compEnd = mAnimationComponents.end();
            while(compIt != compEnd){
                if(auto animator = compIt->lock()){
                    animator->step(update->getLastFrameTime());
                    ++compIt;
                }else{
                    compIt = mAnimationComponents.erase(compIt);
                }
            }
            
            return EventStatus::SUCCESS;
        }

        std::map<std::string,std::weak_ptr<Animatable<float>>> mAnimations;
        std::list<std::weak_ptr<Animator>> mAnimationComponents;
        Scene& mScene;
    };

}//end namespace mediasystem
