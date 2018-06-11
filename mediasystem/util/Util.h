#pragma once

#include "ofMain.h"
#include "mediasystem/util/Singleton.hpp"
#include "mediasystem/util/TimeManager.h"

namespace mediasystem {

	using GlobalTime = Singleton<TimeManager>;

	const size_t getElapsedFrames();
	const double getElapsedSeconds();
	const double getLastFrameTime();

	std::string randString(size_t length);
    
    float& getDebugFontSize();
    std::shared_ptr<ofTrueTypeFont> getDebugFont(float size = getDebugFontSize());

}//end namespace pf
