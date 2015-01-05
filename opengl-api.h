#ifndef OPENGL_API_H
#define OPENGL_API_H


namespace OpenGLAPI
{
    bool LoadSymbols();
    void ClearSymbols();
    void ResetCallCount();
    unsigned int GetCallCount();
};


#endif
