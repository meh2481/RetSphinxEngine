#ifndef OPENGL_API_H
#define OPENGL_API_H

#ifdef _DLL
#define WASDLL
#undef _DLL // HACK: don't put any __declspec in front of the function definitions
#endif

//#define APIENTRY
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>

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


#endif
