
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>

#include <iostream>
#include "opengl-api.h"
#include "easylogging++.h"


// Populate global namespace with static function pointers pFUNC,
// and function stubs FUNC that call their associated function pointer
#define GL_FUNC(ret,fn,params,call,rt) \
    extern "C" { \
    static ret (GLAPIENTRY *p##fn) params = NULL; \
    ret GLAPIENTRY fn params { rt p##fn call; } \
    }
#define GL_PTR(pty, fn) pty fn = NULL;

#include "opengl-stubs.h"


static bool lookup_glsym(const char *funcname, void **func)
{
    *func = SDL_GL_GetProcAddress(funcname);
    if (*func == NULL)
    {
        LOG(ERROR) << "Failed to find OpenGL symbol '" << funcname << "'";
        return false;
    }
    return true;
}

static bool lookup_all_glsyms(void)
{
    bool retval = true;
#define GL_FUNC(ret,fn,params,call,rt) \
    if (!lookup_glsym(#fn, (void **) &p##fn)) retval = false;
#define GL_PTR(_, fn) \
    if (!lookup_glsym(#fn, (void **) &fn)) retval = false;
#include "opengl-stubs.h"
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
    #define GL_PTR(pty, fn) fn = NULL;
    #include "opengl-stubs.h"
}


}; // end namespace OpenGLAPI

