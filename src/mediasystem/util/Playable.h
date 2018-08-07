//
//  Playable.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 5/4/18.
//

#pragma once

#include <stdint.h>
#include <unordered_map>
#include <functional>
#include <limits>

namespace mediasystem {
    
    class Playable {
    public:
        
        struct State {
            
            static State advance( const State& state );
            
            enum {
                PLAYSTATE_NONE = 0,
                PLAYSTATE_PLAYING = 0x01,
                PLAYSTATE_PAUSED = (0x01 << 1),
                PLAYSTATE_LOOPING = (0x01 << 2),
                PLAYSTATE_BOOMARANG = (0x01 << 3),
                PLAYSTATE_REVERSE = (0x01 << 4),
                PLAYSTATE_FINISHED = (0x01 << 5)
            };
            
            uint32_t totalFrames{0};
            uint32_t currentFrame{0};
            uint32_t loopPointIn{std::numeric_limits<uint32_t>::max()};
            uint32_t loopPointOut{std::numeric_limits<uint32_t>::max()};
            uint32_t flags{PLAYSTATE_NONE};
            
            bool operator==(const State& rhs)
            {
                return  (totalFrames == rhs.totalFrames) &&
                (currentFrame == rhs.currentFrame) &&
                (loopPointIn == rhs.loopPointIn) &&
                (loopPointOut == rhs.loopPointOut) &&
                (flags == rhs.flags);
            }
            
            bool operator!=(const State& rhs)
            {
                return !operator==(rhs);
            }
            
            void play();
            void stop();
            void pause();
            
            void reset();
            
            inline void setLoop( const bool flag = true ) { flag ? flags |= PLAYSTATE_LOOPING: flags &= ~PLAYSTATE_LOOPING; }
            inline void setReverse(const bool flag = true) { flag ? flags |= PLAYSTATE_REVERSE: flags &= ~PLAYSTATE_REVERSE; }
            inline void setBoomarang(const bool flag = true) { flag ? flags |= (PLAYSTATE_BOOMARANG | PLAYSTATE_LOOPING): flags &= PLAYSTATE_BOOMARANG; }
            
            inline void setFinished(bool set = true){ set ? flags |= PLAYSTATE_FINISHED : flags &= ~PLAYSTATE_FINISHED; }
            
            inline bool isPlaying() const { return flags & PLAYSTATE_PLAYING; }
            inline bool isReversed() const { return flags & PLAYSTATE_REVERSE; }
            inline bool isPaused() const { return flags & PLAYSTATE_PAUSED; }
            inline bool isLooping() const { return flags & PLAYSTATE_LOOPING; }
            inline bool isBoomarang() const { return flags & PLAYSTATE_PAUSED; }
            inline bool isFinished() const { return flags & PLAYSTATE_FINISHED; }
            inline float getPlaybackPosition() const { return currentFrame/(float)totalFrames; }
            
        };
        
        using KeyFrameCallback = std::function<void(State&)>;
        
        class Options {
        public:
            
            Options():
                mFlags(State::PLAYSTATE_LOOPING),
                mLoopIn(std::numeric_limits<uint32_t>::max()),
                mLoopOut(std::numeric_limits<uint32_t>::max())
            {}
            
            virtual ~Options() = default;
            
            inline Options& disableLooping(){ mFlags &= ~State::PLAYSTATE_LOOPING; return *this; }
            inline Options& keyFrame(uint32_t frame, const KeyFrameCallback& callback){ mKeyFrames.emplace(frame,callback); return *this; }
            inline Options& boomarang(){ mFlags |= (State::PLAYSTATE_LOOPING | State::PLAYSTATE_BOOMARANG); return *this; }
            inline Options& reverse(){ mFlags |= State::PLAYSTATE_REVERSE; return *this; }
            inline Options& loopPoints(uint32_t beginFrame, uint32_t endFrame){ mLoopIn = beginFrame, mLoopOut = endFrame; return *this; }
            inline uint32_t getFlags()const{return mFlags;}
            inline const std::unordered_map<uint32_t,KeyFrameCallback>& getKeyFrames() const { return mKeyFrames; }
            inline std::pair<uint32_t, uint32_t> getLoopPoints() const { return {mLoopIn, mLoopOut}; }
            
        private:
            uint32_t mFlags;
            uint32_t mLoopIn, mLoopOut;
            std::unordered_map<uint32_t,KeyFrameCallback> mKeyFrames;
        };
        
        explicit Playable( float fps, const Options& options = Options() );
        virtual ~Playable() = default;
        
        virtual void play();
        virtual void stop();
        virtual void pause();
        virtual void advanceFrame();
        virtual void step(double amount);
        virtual void reset();
        
        void setKeyFrame(uint32_t frame, const KeyFrameCallback& callback);
        
        inline const State& getState()const{return mState;}
        
        inline virtual bool isPaused()const { return mState.isPaused(); }
        inline virtual bool isPlaying()const { return mState.isPlaying(); }
        inline virtual bool isReversed()const { return mState.isReversed(); }
        inline virtual bool isLooping()const { return mState.isLooping(); }
        inline virtual bool isBoomarang()const { return mState.isBoomarang(); }
        inline virtual bool isFinished()const { return mState.isFinished(); }

        inline virtual void setLoop(bool flag = true) { mState.setLoop(flag); }
        inline virtual void setReverse(bool flag = true) { mState.setReverse(flag); }
        inline virtual void setBoomarang(bool flag = true) { mState.setBoomarang(flag); }
        inline virtual void setLoopPointIn(uint32_t frame){ mState.loopPointIn = frame; }
        inline virtual void setLoopPointOut(uint32_t frame){ mState.loopPointOut = frame; }
        inline virtual void setLoopPoints(uint32_t inFrame, uint32_t outFrame ){ mState.loopPointIn = inFrame; mState.loopPointOut = outFrame; }
        
        inline virtual void setFinished(bool flag = true) { mState.setFinished(flag); }
        
        inline float getPlaybackPosition() const { return getCurrentFrame()/(float)getTotalFrames(); }
        
        inline uint32_t getCurrentFrame() const { return mState.currentFrame; }
        inline uint32_t getTotalFrames() const { return mState.totalFrames; }
        inline uint32_t getLoopPointIn() const { return mState.loopPointIn; }
        inline uint32_t getLoopPointOut() const { return mState.loopPointOut; }
        inline float getDuration() const { return getTotalFrames()/(float)getFramerate(); }
        inline virtual void setFramerate(const float rate) { mFramerate = rate; }
        inline float getFramerate() const { return mFramerate; }

    protected:
        
        float mFramerate;
        double mElapsedTime;
        State mState;
        std::unordered_map<uint32_t,KeyFrameCallback> mKeyFrames;

    };
    
}// end namespace mediasystem
