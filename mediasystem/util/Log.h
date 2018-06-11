//
//  Log.h
//  ProjectionDemo
//
//  Created by Michael Allison on 4/25/18.
//

#pragma once

#include "ofLog.h"

#define MS_LOG_INFO(message)\
    ofLogNotice() << message;

#define MS_LOG_VERBOSE(message)\
    ofLogVerbose() << message;

#define MS_LOG_WARNING(message)\
    ofLogWarning() << message;

#define MS_LOG_ERROR(message)\
    ofLogError() << message;

#define MS_LOG_FATAL(message)\
    ofLogFatalError() << message;
