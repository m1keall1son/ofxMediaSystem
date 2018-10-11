//
//  glsl_define.h
//  WallTest
//
//  Created by Michael Allison on 6/22/18.
//

#pragma once
#include "ofMain.h"

#define OF_GLSL(vers, code) "#version "#vers"\n "#code

namespace mediasystem {
    inline void checkGLError() {
        GLenum err;
        bool noError = true;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            noError = false;
            ofLogError() << " glError : " << gluErrorString(err);
        }
        assert(noError);
    }
}

#if ! defined( NDEBUG )
#define OF_CHECK_GL_ERROR()    mediasystem::checkGLError()
#else
#define OF_CHECK_GL_ERROR()    ((void)0)
#endif
