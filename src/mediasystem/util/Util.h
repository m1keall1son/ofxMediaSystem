#pragma once

#include "ofMain.h"
#include "mediasystem/util/Singleton.hpp"
#include "mediasystem/util/TimeManager.h"

namespace mediasystem {

	std::string randString(size_t length);
    
    float& getDebugFontSize();
    std::shared_ptr<ofTrueTypeFont> getDebugFont(float size = getDebugFontSize());

}//end namespace pf
