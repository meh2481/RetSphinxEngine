#pragma once

#ifdef _DLL
#define WASDLL
#undef _DLL // HACK: don't put any __declspec in front of the function definitions
#endif

#include <SDL_opengl.h>

#define GL_FUNC(ret,fn,params,call,rt)
#define GL_PTR(pty, fn) extern pty fn;
#include "opengl-stubs.h"


#ifdef WASDLL
#define _DLL
#undef WASDLL
#endif


namespace OpenGLAPI
{
    bool LoadSymbols();
    void ClearSymbols();
    void ResetCallCount();
    unsigned int GetCallCount();
};

