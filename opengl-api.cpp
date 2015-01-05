#ifdef USE_SDL_FRAMEWORK
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>
#include "opengl-api.h"

#ifdef _WIN32
#define GLAPIENTRY __stdcall
#else
#define GLAPIENTRY
#endif

// Populate global namespace with static function pointers pFUNC,
// and function stubs FUNC that call their associated function pointer
#define GL_FUNC(ret,fn,params,call,rt) \
    extern "C" { \
    static ret (GLAPIENTRY *p##fn) params = NULL; \
    ret fn params { rt p##fn call; } \
    }

#include "opengl-stubs.h"
#undef GL_FUNC

static bool lookup_glsym(const char *funcname, void **func)
{
    *func = SDL_GL_GetProcAddress(funcname);
    if (*func == NULL)
    {
        std::cerr << "Failed to find OpenGL symbol '" << funcname << "'" << std::endl;
        return false;
    }
    return true;
}

static bool lookup_all_glsyms(void)
{
    bool retval = true;
#define GL_FUNC(ret,fn,params,call,rt) \
    if (!lookup_glsym(#fn, (void **) &p##fn)) retval = false;
#include "opengl-stubs.h"
#undef GL_FUNC
    return retval;
}



namespace OpenGLAPI {


bool LoadSymbols()
{
    return lookup_all_glsyms();
}

void ClearSymbols()
{
    // reset all the entry points to NULL, so we know exactly what happened
    //  if we call a GL function after shutdown.
    #define GL_FUNC(ret,fn,params,call,rt) p##fn = NULL;
    #include "opengl-stubs.h"
    #undef GL_FUNC
}


}; // end namespace OpenGLAPI

