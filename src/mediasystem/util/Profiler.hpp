//
//  Profiler.hpp
//  ProjectionDemo
//
//  Created by Michael Allison on 4/27/18.
//

#pragma once

#include "mediasystem/util/Log.h"
#include "mediasystem/util/BasicTimer.hpp"

#if defined(MS_ALLOW_PROFILE)
#define MS_PROFILE_START(clockname)\
mediasystem::Timer clockname##_timer(true);

#define MS_PROFILE_STOP(clockname)\
MS_LOG_INFO("profiler: " << #clockname << " elapsed nanoseconds: " << clockname##_timer.getNanos());
#else
#define MS_PROFILE_START(clockname) ((void)0)
#define MS_PROFILE_STOP(clockname) ((void)0)
#endif
