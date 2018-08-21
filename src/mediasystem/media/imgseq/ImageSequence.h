#pragma once

#include "ofMain.h"
#include "mediasystem/util/Playable.hpp"

namespace mediasystem {
    
    class ImageSequenceBase : public Playable<uint32_t> {
    public:
        
        ImageSequenceBase(std::filesystem::path&& pathToImgDir, float fps, const Playable<uint32_t>::Options& options);
        virtual ~ImageSequenceBase() = default;
        
        virtual void load() = 0;
        virtual void update(float amount);
        virtual ofTexture* getCurrentTexture() = 0;
        virtual glm::vec2 getSize() const  = 0;
        
        bool isLoaded() const { return mIsInit; }
        
        inline void setTextureLocation(int bindLocation){ mTextureLocation = bindLocation; }
        void bind();
        void unbind();
        
        inline void setFramerate(float fps){ mFramerate = fps; }
        inline float getFramerate() const { return mFramerate; }
        
    protected:
                
        static void parseSource(const std::filesystem::path& path, std::vector<std::filesystem::path>& imageFiles, const std::vector<std::filesystem::path> allowedExt = { ".png", ".jpg", ".gif", ".JPG", ".PNG", ".jpeg", ".JPEG" });
        
        std::filesystem::path mImagesDir;
        std::vector<std::filesystem::path> mSeqPaths;
        bool mIsInit{false};
        int mTextureLocation{0};
        float mFramerate{60.f};
        float mInvFps{1.f/60.f};
        float mElapsedFrameTime{0.f};
    };
    
    class ImageSequence {
    public:
        
        enum Type { SEQ_PRELOAD, SEQ_DISK_STREAMING };
        
        class Options {
        public:
            
            Options():mType(SEQ_PRELOAD){}
            
            inline Options& streaming(){ mType = SEQ_DISK_STREAMING; return *this; }
            inline Options& preload(){ mType = SEQ_PRELOAD; return *this; }
            inline Options& loop(){ mPlayableOptions.loop(); return *this; }
            inline Options& keyFrame(uint32_t frame, const Playable<uint32_t>::KeyFrameCallback& callback){ mPlayableOptions.keyFrame(frame,callback); return *this; }
            inline Options& palindrome(){ mPlayableOptions.palindrome(); return *this; }
            inline Options& reverse(){ mPlayableOptions.reverse(); return *this; }
            inline Options& loopPoints(uint32_t beginFrame, uint32_t endFrame){ mPlayableOptions.loopPoints(beginFrame,endFrame); return *this; }
            
        private:
            ImageSequence::Type mType;
            Playable<uint32_t>::Options mPlayableOptions;
            friend ImageSequence;
        };
        
        ImageSequence(std::filesystem::path pathToImgDir, float fps, const ImageSequence::Options& options = ImageSequence::Options());
        
        inline void load(){ return mImpl->load(); };
        inline ofTexture* getCurrentTexture(){ return mImpl->getCurrentTexture(); }
        inline glm::vec2 getSize() const { return mImpl->getSize(); }
        inline bool isLoaded() const { return mImpl->isLoaded(); }
        inline void setTextureLocation(int bindLocation){ mImpl->setTextureLocation(bindLocation); }
        inline void bind(){ mImpl->bind(); }
        inline void unbind(){ mImpl->unbind(); }
        inline void play(){ mImpl->play(); }
        inline void stop(){ mImpl->stop(); }
        inline void pause(){ mImpl->pause(); }
        inline void update(double amount){ mImpl->update(amount); }
        inline void reset(){ mImpl->reset(); }
        
        inline void setKeyFrame(uint32_t frame, const Playable<uint32_t>::KeyFrameCallback& callback){ mImpl->setKeyFrame(frame, callback); }
        
        inline const Playable<uint32_t>::State& getState()const{ return mImpl->getState(); }
        
        inline bool isPaused()const { return mImpl->isPaused(); }
        inline bool isPlaying()const { return mImpl->isPlaying(); }
        inline bool isReversed()const { return mImpl->isReversed(); }
        inline bool isLooping()const { return mImpl->isLooping(); }
        inline bool isPalindrome()const { return mImpl->isPalindrome(); }
        inline bool isFinished()const { return mImpl->isFinished(); }
        
        inline void setLoop(bool flag = true){ mImpl->setLoop(flag); }
        inline void setReverse(bool flag = true){ mImpl->setReverse(flag); }
        inline void setPalindrome(bool flag = true){ mImpl->setPalindrome(flag); }
        inline void setLoopPointIn(uint32_t frame){ mImpl->setLoopPointIn(frame); }
        inline void setLoopPointOut(uint32_t frame){ mImpl->setLoopPointOut(frame); }
        inline void setLoopPoints(uint32_t inFrame, uint32_t outFrame ){ mImpl->setLoopPoints(inFrame, outFrame); }
        
        inline void setFinished(bool flag = true){ mImpl->setFinished(flag); }
        
        inline float getPlaybackPosition() const { return mImpl->getCurrentPosition() / (float) getCurrentFrame(); }
        
        inline uint32_t getCurrentFrame() const { return mImpl->getCurrentPosition(); }
        inline uint32_t getTotalFrames() const { return mImpl->getDuration(); }
        inline uint32_t getLoopPointIn() const { return mImpl->getLoopPointIn(); }
        inline uint32_t getLoopPointOut() const { return mImpl->getLoopPointOut(); }
        inline float getDuration() const { return mImpl->getDuration() * (1.f / mImpl->getFramerate()); }
        inline void setFramerate(const float rate) { mImpl->setFramerate(rate); }
        inline float getFramerate() const { return mImpl->getFramerate(); }
        
    private:
        Type mType{SEQ_PRELOAD};
        std::unique_ptr<ImageSequenceBase> mImpl;
    };

}//end namespace mediasystem
