#include "Util.h"
#include <random>
#include <string>

namespace mediasystem {
    
    const size_t getElapsedFrames()
    {
        auto tc = GlobalTime::get();
        return tc->getElapsedFrames();
    }

    const double getElapsedSeconds()
    {
        auto tc = GlobalTime::get();
        return tc->getElapsedSeconds();
    }

    const double getLastFrameTime()
    {
        auto tc = GlobalTime::get();
        return tc->getLastFrameTime();
    }

    std::string randString(size_t length)
    {
        auto randchar = []() -> char
        {
            const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[(int)ofRandom(max_index)];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }
    
    static std::map<float, std::shared_ptr<ofTrueTypeFont>> sDebugFonts;
    static float sGlobalDebugFontSize = 18.f;
    
    float& getDebugFontSize()
    {
        return sGlobalDebugFontSize;
    }
    
    std::shared_ptr<ofTrueTypeFont> getDebugFont(float size)
    {
        auto found = sDebugFonts[size];
        if(!found){
            found.reset(new ofTrueTypeFont);
            found->load(ofToDataPath("fonts/VeraMono.ttf"), size, true, true, true);
        }
        return found;
    }
    
}//end namespcae mediasystem
