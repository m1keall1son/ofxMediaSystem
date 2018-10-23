//
//  Playable2.hpp
//  Aerialtronics
//
//  Created by Michael Allison on 8/10/18.
//

#pragma once

#include <stdint.h>
#include <functional>
#include <limits>
#include <type_traits>
#include <list>

namespace mediasystem {

    template<typename T>
    class Playable {
    public:
        
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "T can only be an integral or floating point type.");
        
        struct State {
            
            static State advance( const State& state, T amount ){
                Playable::State ret = state;
                
                auto loopStart = ret.loopPointIn == std::numeric_limits<T>::max() ? 0 : ret.loopPointIn;
                auto loopEnd = ret.loopPointOut == std::numeric_limits<T>::max() ? ret.duration : ret.loopPointOut;
                
                if (ret.flags & PLAYSTATE_LOOPING) {
                    if (ret.flags & PLAYSTATE_REVERSE) {
                        if (ret.currentPosition <= loopStart) {
                            if (state.flags & PLAYSTATE_PALINDROME) {
                                ret.flags ^= PLAYSTATE_REVERSE;
                                ret.currentPosition += amount;
                            }
                            else {
                                ret.currentPosition = loopEnd;
                            }
                        }else{
                            ret.currentPosition -= amount;
                        }
                    }
                    else {
                        if (ret.currentPosition >= loopEnd) {
                            if (ret.flags & PLAYSTATE_PALINDROME) {
                                ret.flags ^= PLAYSTATE_REVERSE;
                                ret.currentPosition -= amount;
                            }
                            else {
                                ret.currentPosition = loopStart;
                            }
                        }else{
                            ret.currentPosition += amount;
                        }
                    }
                }
                else {
                    if (ret.flags & PLAYSTATE_REVERSE) {
                        if (ret.currentPosition <= 0) {
                            ret.currentPosition = ret.duration;
                            ret.flags |= PLAYSTATE_FINISHED;
                        }else{
                            ret.currentPosition -= amount;
                        }
                    }
                    else {
                        if (ret.currentPosition >= ret.duration) {
                            ret.currentPosition = 0;
                            ret.flags |= PLAYSTATE_FINISHED;
                        }else{
                            ret.currentPosition += amount;
                        }
                    }
                }
                return ret;
            }
            
            enum {
                PLAYSTATE_NONE = 0,
                PLAYSTATE_PLAYING = 0x01,
                PLAYSTATE_PAUSED = (0x01 << 1),
                PLAYSTATE_LOOPING = (0x01 << 2),
                PLAYSTATE_PALINDROME = (0x01 << 3),
                PLAYSTATE_REVERSE = (0x01 << 4),
                PLAYSTATE_FINISHED = (0x01 << 5)
            };
            
            T duration{0};
            T currentPosition{0};
            T loopPointIn{std::numeric_limits<T>::max()};
            T loopPointOut{std::numeric_limits<T>::max()};
            uint32_t flags{PLAYSTATE_NONE};
            
            bool operator==(const State& rhs)
            {
                return  (duration == rhs.duration) &&
                (currentPosition == rhs.currentPosition) &&
                (loopPointIn == rhs.loopPointIn) &&
                (loopPointOut == rhs.loopPointOut) &&
                (flags == rhs.flags);
            }
            
            bool operator!=(const State& rhs)
            {
                return !operator==(rhs);
            }
            
            void play()
            {
                flags &= ~PLAYSTATE_FINISHED;
                
                if (flags & PLAYSTATE_PAUSED) {
                    flags &= ~PLAYSTATE_PAUSED;
                }
                else {
                    if (duration > 0) {
                        flags &= ~PLAYSTATE_PAUSED;
                        flags |= PLAYSTATE_PLAYING;
                    }
                }
            }
            
            void stop()
            {
                flags &= ~PLAYSTATE_PLAYING;
                flags &= ~PLAYSTATE_PAUSED;
            }
            
            void reset()
            {
                if ((flags & PLAYSTATE_REVERSE) && !(flags & PLAYSTATE_PALINDROME)) {
                    currentPosition = duration;
                }
                else {
                    currentPosition = 0;
                }
            }
            
            void pause() { flags |= PLAYSTATE_PAUSED; }
            
            void setLoop( const bool flag = true ) { flag ? flags |= PLAYSTATE_LOOPING: flags &= ~PLAYSTATE_LOOPING; }
            void setReverse(const bool flag = true) { flag ? flags |= PLAYSTATE_REVERSE: flags &= ~PLAYSTATE_REVERSE; }
            void setPalindrome(const bool flag = true) { flag ? flags |= (PLAYSTATE_PALINDROME | PLAYSTATE_LOOPING): flags &= PLAYSTATE_PALINDROME; }
            
            void setFinished(bool set = true){ set ? flags |= PLAYSTATE_FINISHED : flags &= ~PLAYSTATE_FINISHED; }
            
            bool isPlaying() const { return flags & PLAYSTATE_PLAYING; }
            bool isReversed() const { return flags & PLAYSTATE_REVERSE; }
            bool isPaused() const { return flags & PLAYSTATE_PAUSED; }
            bool isLooping() const { return flags & PLAYSTATE_LOOPING; }
            bool isPalindrome() const { return flags & PLAYSTATE_PALINDROME; }
            bool isFinished() const { return flags & PLAYSTATE_FINISHED; }
            float getPercentComplete() const { return currentPosition/(float)duration; }
            
        };
        
        using KeyFrameCallback = std::function<void(State&)>;
        struct KeyFrame {
            KeyFrame(T pos, KeyFrameCallback cb):
            position(pos),
            callback(std::move(cb))
            {}
            T position;
            KeyFrameCallback callback;
        };
        
        static void insertKeyframe(KeyFrame&& keyframe, std::list<KeyFrame>& keyframes){
            auto found = std::find_if(keyframes.begin(), keyframes.end(), [&keyframe](const KeyFrame& kf){
                return kf.position > keyframe.position;
            });
            if(found != keyframes.end()){
                keyframes.emplace(found, std::move(keyframe));
            }else{
                keyframes.emplace_back(std::move(keyframe));
            }
        }
        
        class Options {
        public:
            
            Options():
            mFlags(State::PLAYSTATE_NONE),
            mLoopIn(std::numeric_limits<T>::max()),
            mLoopOut(std::numeric_limits<T>::max())
            {}
            
            ~Options() = default;
            
            Options& loop(){ mFlags |= State::PLAYSTATE_LOOPING; return *this; }
            Options& keyFrame(T position, KeyFrameCallback callback){
                insertKeyframe(KeyFrame(position, std::move(callback)), mKeyFrames);
                return *this;
            }
            Options& palindrome(){ mFlags = mFlags | State::PLAYSTATE_LOOPING | State::PLAYSTATE_PALINDROME; return *this; }
            Options& reverse(){ mFlags |= State::PLAYSTATE_REVERSE; return *this; }
            Options& loopPoints(T in, T out){ mLoopIn = in, mLoopOut = out; return *this; }
            Options& delay(T amount){ mDelay = amount; return *this; }
            Options& playbackSpeed(T speed){ mSpeed = speed; return *this; }
            //after delay has passed, but only if there is one
            Options& startAfterDelayFn(std::function<void()> afterDelay){ mOnStartAfterDelay = std::move(afterDelay); return *this; }
            //on play()
            Options& startFn(std::function<void()> start){ mOnStartAfterDelay = std::move(start); return *this; }
            Options& stopFn(std::function<void()> stop){ mOnStopFn = std::move(stop); return *this; }
            Options& finishFn(std::function<void()> finish){ mOnFinishFn = std::move(finish); return *this; }
            Options& updateFn(std::function<void()> update){ mOnUpdateFn = std::move(update); return *this; }
            Options& loopedFn(std::function<void()> looped){ mOnLoopedFn = std::move(looped); return *this; }
            
            uint32_t getFlags()const{return mFlags;}
            const std::list<KeyFrame>& getKeyFrames() const { return mKeyFrames; }
            bool isReversed() const { return mFlags & Playable<T>::State::PLAYSTATE_REVERSE; }
            bool isLooping() const { return mFlags & Playable<T>::State::PLAYSTATE_LOOPING; }
            bool isPalindrome() const { return mFlags & Playable<T>::State::PLAYSTATE_PALINDROME; }
            bool hasDelay() const { return mDelay > 0; }
            std::pair<T, T> getLoopPoints() const { return {mLoopIn, mLoopOut}; }
            T getDelay() const { return mDelay; }
            std::function<void()>& getStartFn(){ return mOnStartFn; }
            std::function<void()>& getStartAfterDelayFn(){ return mOnStartAfterDelay; }
            std::function<void()>& getStopFn(){ return mOnStopFn; }
            std::function<void()>& getFinishFn(){ return mOnFinishFn; }
            std::function<void()>& getUpdateFn(){ return mOnUpdateFn; }
            std::function<void()>& getLoopedFn(){ return mOnLoopedFn; }
            
        private:
            uint32_t mFlags;
            T mLoopIn{std::numeric_limits<T>::max()}, mLoopOut{std::numeric_limits<T>::max()};
            T mDelay{0};
            T mSpeed{1};
            std::list<KeyFrame> mKeyFrames;
            std::function<void()> mOnStartAfterDelay;
            std::function<void()> mOnStartFn;
            std::function<void()> mOnStopFn;
            std::function<void()> mOnFinishFn;
            std::function<void()> mOnUpdateFn;
            std::function<void()> mOnLoopedFn;
            friend Playable;
        };
        
        virtual ~Playable() = default;
        
        Playable(T duration, const Options& options = Options())
        {
            init(duration,options);
        }
        
        virtual void play(){
            reset();
            mState.play();
            if(mHandlersMap[ON_START_HANDLER]){
                mHandlersMap[ON_START_HANDLER]();
            }
            if(mDelay > 0 && mDelayElapsed == 0){
                mWaitingOnDelay = true;
            }
        }
        
        virtual void pause() { mState.pause(); }
        
        virtual void stop() {
            mState.stop();
            if(mHandlersMap[ON_STOP_HANDLER]){
                mHandlersMap[ON_STOP_HANDLER]();
            }
        }
        
        virtual void reset(){
            mState.reset();
            mDelayElapsed = 0;
            mWaitingOnDelay = false;
            resetKeyFrames();
        }
        
        virtual void setDuration(T duration){
            mState.reset();
            resetKeyFrames();
            mState.duration = duration;
            if(mState.loopPointOut > duration){
                mState.loopPointOut = duration;
            }
        }
        
        virtual void init(T duration, const Options& options = Options()){
            mState.flags = options.getFlags();
            auto loopPoints = options.getLoopPoints();
            
            if(loopPoints.first == std::numeric_limits<T>::max()){
                mState.loopPointIn = 0;
            }else{
                mState.loopPointIn = loopPoints.first;
            }
            
            if(loopPoints.second == std::numeric_limits<T>::max()){
                mState.loopPointOut = duration;
            }else{
                mState.loopPointOut = loopPoints.second;
            }
            
            mState.duration = duration;
            if(!options.getKeyFrames().empty())
                mKeyFrames = std::move(options.getKeyFrames());
            mDelay = options.mDelay;
            mSpeed = options.mSpeed;
            
            if(options.mOnStartFn)
                mHandlersMap[ON_START_HANDLER] = std::move(options.mOnStartFn);
            if(options.mOnStartAfterDelay)
                mHandlersMap[ON_DELAYED_START_HANDLER] = std::move(options.mOnStartAfterDelay);
            if(options.mOnFinishFn)
                mHandlersMap[ON_FINISH_HANDLER] = std::move(options.mOnFinishFn);
            if(options.mOnUpdateFn)
                mHandlersMap[ON_UPDATE_HANDLER] = std::move(options.mOnUpdateFn);
            if(options.mOnLoopedFn)
                mHandlersMap[ON_LOOPED_HANDLER] = std::move(options.mOnLoopedFn);
            if(options.mOnStopFn)
                mHandlersMap[ON_STOP_HANDLER] = std::move(options.mOnStopFn);
            
            resetKeyFrames();
        }
        
        virtual void clearKeyFrames(){
            mKeyFrames.clear();
            mCurrentKeyFrame = mKeyFrames.begin();
        }
        
        virtual void setKeyFrame(T position, KeyFrameCallback callback){
            insertKeyframe(KeyFrame(position, std::move(callback)), mKeyFrames);
        }
        
        virtual void step(T amount){
            swapHandlers(); //swap out if neccessary
            if ((mState.isPlaying()) && !(mState.isPaused())) {
                
                //check delay
                if(mDelayElapsed >= mDelay){
                    if(mWaitingOnDelay){
                        if(mHandlersMap[ON_DELAYED_START_HANDLER])
                            mHandlersMap[ON_DELAYED_START_HANDLER]();
                        mWaitingOnDelay = false;
                    }
                    
                    //advance state
                    auto prevPosition = mState.currentPosition;
                    auto prevReverse = mState.isReversed();
                    mState = State::advance(mState, amount * mSpeed);
                    if(mState.isFinished()){
                        stop();
                        if(mHandlersMap[ON_FINISH_HANDLER])
                            mHandlersMap[ON_FINISH_HANDLER]();
                        return;
                    }
                    
                    if(mHandlersMap[ON_UPDATE_HANDLER])
                        mHandlersMap[ON_UPDATE_HANDLER]();
                    
                    //check loop
                    if(mState.isLooping()){
                        if(mState.isPalindrome()){
                            if(mState.isReversed() != prevReverse){
                                if(mHandlersMap[ON_LOOPED_HANDLER])
                                    mHandlersMap[ON_LOOPED_HANDLER]();
                                resetKeyFrames();
                            }
                        }else{
                            if(mState.isReversed()){
                                if(mState.currentPosition > prevPosition){
                                    if(mHandlersMap[ON_LOOPED_HANDLER])
                                        mHandlersMap[ON_LOOPED_HANDLER]();
                                    resetKeyFrames();
                                }
                            }else{
                                if(mState.currentPosition < prevPosition){
                                    if(mHandlersMap[ON_LOOPED_HANDLER])
                                        mHandlersMap[ON_LOOPED_HANDLER]();
                                    resetKeyFrames();
                                }
                            }
                        }
                    }
                    
                    //check keyframes
                    if(!mKeyFrames.empty() && mCurrentKeyFrame != mKeyFrames.end()){
                        if(mState.isReversed()){
                            if( mState.currentPosition <= mCurrentKeyFrame->position){
                                mCurrentKeyFrame->callback(mState);
                                if(mCurrentKeyFrame == mKeyFrames.begin()){
                                    mCurrentKeyFrame = mKeyFrames.end();
                                }else{
                                    auto it = --mCurrentKeyFrame;
                                    bool done = false;
                                    while(!done){
                                        if(mState.currentPosition <= it->position){
                                            mCurrentKeyFrame->callback(mState);
                                            if(mCurrentKeyFrame == mKeyFrames.begin()){
                                                done = true;
                                            }else{
                                                mCurrentKeyFrame = --it;
                                            }
                                        }else{
                                            done = true;
                                        }
                                    }
                                }
                            }
                        }else{
                            if(mState.currentPosition >= mCurrentKeyFrame->position){
                                mCurrentKeyFrame->callback(mState);
                                auto it = ++mCurrentKeyFrame;
                                while(it != mKeyFrames.end()){
                                    if(mState.currentPosition >= it->position){
                                        mCurrentKeyFrame->callback(mState);
                                        mCurrentKeyFrame = ++it;
                                    }else{
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }else{
                    mDelayElapsed += amount;
                }
            }
        }
        
        virtual bool isPaused()const { return mState.isPaused(); }
        virtual bool isPlaying()const { return mState.isPlaying(); }
        virtual bool isReversed()const { return mState.isReversed(); }
        virtual bool isLooping()const { return mState.isLooping(); }
        virtual bool isPalindrome()const { return mState.isPalindrome(); }
        virtual bool isFinished()const { return mState.isFinished(); }
        virtual bool isWaitingOnDelay() const { return mWaitingOnDelay; }

        virtual void setLoop(bool flag = true) { mState.setLoop(flag); }
        virtual void setReverse(bool flag = true) { mState.setReverse(flag); }
        virtual void setPalindrome(bool flag = true) { mState.setPalindrome(flag); }
        virtual void setLoopPointIn(T frame){ mState.loopPointIn = frame; }
        virtual void setLoopPointOut(T frame){ mState.loopPointOut = frame; }
        virtual void setLoopPoints(T inFrame, T outFrame ){ mState.loopPointIn = inFrame; mState.loopPointOut = outFrame; }
        virtual void setDelay(T delay){ mDelay = delay; mDelayElapsed = 0; }
        virtual void setFinished(bool flag = true) { mState.setFinished(flag); }
        virtual void setPlaybackSpeed(T speed){ mSpeed = speed; }
        
        //only if there's a delay
        virtual void setStartAfterDelayFn(std::function<void()> afterDelay){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_DELAYED_START_HANDLER, std::move(afterDelay) );
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        virtual void clearStartAfterDelayFn(){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_DELAYED_START_HANDLER, nullptr);
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        
        //always immediatly on play()
        virtual void setStartFn(std::function<void()> start){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_START_HANDLER, std::move(start) );
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        virtual void clearStartFn(){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_START_HANDLER, nullptr);
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        
        virtual void setStopFn(std::function<void()> stop){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_STOP_HANDLER, std::move(stop) );
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        virtual void clearStopFn(){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_STOP_HANDLER, nullptr);
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        
        virtual void setFinishFn(std::function<void()> finish){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_FINISH_HANDLER, std::move(finish) );
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        virtual void clearFinishFn(){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_FINISH_HANDLER, nullptr);
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        
        virtual void setUpdateFn(std::function<void()> update){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_UPDATE_HANDLER, std::move(update) );
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        virtual void clearUpdateFn(){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_UPDATE_HANDLER, nullptr);
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        
        //triggers for regular loop and plaindrome
        virtual void setLoopedFn(std::function<void()> looped){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_LOOPED_HANDLER, std::move(looped) );
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        virtual void clearLoopedFn(){
            std::pair<Handler,std::function<void()>> p = std::make_pair( ON_LOOPED_HANDLER, nullptr);
            mUpdateHandlerQueue.emplace_back(std::move(p));
        }
        
        const State& getState()const{return mState;}
        float getPercentComplete() const { return getCurrentPosition()/(float)getDuration(); }
        T getCurrentPosition() const { return mState.currentPosition; }
        T getDuration() const { return mState.duration; }
        T getLoopPointIn() const { return mState.loopPointIn; }
        T getLoopPointOut() const { return mState.loopPointOut; }
        T getDelay() const { return mDelay; }
        T getPlaybackSpeed() const { return mSpeed; }
        
    protected:
        
        enum Handler {
            ON_START_HANDLER,
            ON_DELAYED_START_HANDLER,
            ON_STOP_HANDLER,
            ON_FINISH_HANDLER,
            ON_UPDATE_HANDLER,
            ON_LOOPED_HANDLER
        };
        
        void swapHandlers(){
            for(auto & update : mUpdateHandlerQueue){
                std::swap(mHandlersMap[update.first], update.second);
            }
            mUpdateHandlerQueue.clear();
        }
        
        void resetKeyFrames(){
            if(mState.isReversed() && mKeyFrames.size() > 1){
                mCurrentKeyFrame = mKeyFrames.end();
                --mCurrentKeyFrame;
            }else{
                mCurrentKeyFrame = mKeyFrames.begin();
            }
        }
        
        bool mWaitingOnDelay{false};
        T mDelayElapsed{0};
        T mDelay{0};
        T mSpeed{1};
        State mState;
        std::list<KeyFrame> mKeyFrames;
        typename std::list<KeyFrame>::iterator mCurrentKeyFrame;
        //todo, think about the allocation strategy here?
        std::map<Handler,std::function<void()>> mHandlersMap;
        std::vector<std::pair<Handler,std::function<void()>>> mUpdateHandlerQueue;
    };
    
}//end namespace mediasystem
