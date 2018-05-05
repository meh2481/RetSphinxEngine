#include "GameEngine.h"
#include <ctime>
#include <climits>
#include <iomanip>
#include "Logger.h"
#include <sstream>
#include "DebugUI.h"
#include "ResourceLoader.h"
#include "ResourceCache.h"
#include "ParticleSystem.h"
#include "ParticleEditor.h"
#include "InputManager.h"
#ifdef _DEBUG
    #include "InterpolationManager.h"
#endif

typedef struct
{
    int w, h;
    unsigned char* pixels;
    std::string filename;
} printscreenData;


void GameEngine::handleEvent(SDL_Event event)
{
    // Input that happens no matter what
    switch(event.type)
    {
        //Key pressed
        case SDL_KEYDOWN:
            switch(event.key.keysym.scancode)
            {
#ifdef _DEBUG
                case SDL_SCANCODE_F5:
                    Lua->call("clearClasses"); //Reload Lua classes
                    getResourceLoader()->clearCache();  //Force-clear cache
                    Lua->call("loadLua", m_sLastScene.c_str());    //Restart Lua with our last map
                    break;

                case SDL_SCANCODE_F6:
                    Lua->call("clearClasses"); //Reload Lua classes
                    getResourceLoader()->clearCache();  //Force-clear cache
                    Lua->call("loadLua");    //Restart Lua
                    break;
#endif //_DEBUG

                case SDL_SCANCODE_ESCAPE:
                    quit();
                    break;

                case SDL_SCANCODE_RETURN:    //Alt-Enter toggles fullscreen
                    if(getInputManager()->keyDown(SDL_SCANCODE_ALT))
                        setFullscreen(!isFullscreen());
                    break;

                case SDL_SCANCODE_GRAVE: // Use the traditional quake key for debug console stuff
#ifdef _DEBUG
                    if(m_debugUI->visible)
                        playPhysics();
#endif
                    m_debugUI->visible = !m_debugUI->visible;
                    break;
            }

        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    m_bMouseGrabOnWindowRegain = isMouseGrabbed();
                    grabMouse(false);    //Ungrab mouse cursor if alt-tabbing out or such
                    break;

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    grabMouse(m_bMouseGrabOnWindowRegain);    //Grab mouse on input regain, if we should
                    break;
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            LOG_info("Controller %d pressed button %d", (int)event.cbutton.which, (int)event.cbutton.button);
            getInputManager()->activateController(event.cbutton.which);
            switch(event.cbutton.button)
            {
                case SDL_CONTROLLER_BUTTON_BACK:    //TODO Not hardcoded
                    quit();
                    break;
            }
            break;

        case SDL_CONTROLLERBUTTONUP:
            LOG_trace("Controller %d released button %d", (int)event.cbutton.which, (int)event.cbutton.button);
            break;

        case SDL_CONTROLLERAXISMOTION:
            if(abs(event.caxis.value) > JOY_AXIS_TRIP)
                LOG_trace("Controller %d moved axis %d to %d", (int)event.caxis.which, (int)event.caxis.axis, event.caxis.value);
            break;
    }


    switch(event.type)
    {
        //Key pressed
        case SDL_KEYDOWN:
#ifdef _DEBUG
            if(m_debugUI->visible)
                break;
            switch(event.key.keysym.scancode)
            {
                case SDL_SCANCODE_V:
                    toggleDebugDraw();
                    break;

                case SDL_SCANCODE_B:
                    toggleSoundDebugDraw();
                    break;

                case SDL_SCANCODE_P:
                    playPausePhysics();
                    break;

                case SDL_SCANCODE_O:
                    pausePhysics();
                    stepPhysics();
                    break;
            }
#endif
            break;

        //Key released
        case SDL_KEYUP:
            //switch(event.key.keysym.scancode)
            //{
            //}
            break;

        case SDL_MOUSEBUTTONDOWN:
#ifdef _DEBUG
            if(event.button.button == SDL_BUTTON_RIGHT && m_debugUI->particleEditor->open && m_debugUI->visible)
                m_debugUI->particleEditor->particles->emitFrom.centerOn(worldPosFromCursor(Vec2(event.button.x, event.button.y), Vec3(0.0f, 0.0f, m_fDefCameraZ)));
#endif
            LOG_trace("Mouse button %d pressed.");
            break;

        case SDL_MOUSEWHEEL:
            if(!m_debugUI->visible)
            {
                if(event.wheel.y > 0)
                    cameraPos.z += 1.5;// min(cameraPos.z + 1.5, -5.0);
                else
                    cameraPos.z -= 1.5;// max(cameraPos.z - 1.5, -3000.0);
            }
            break;

        case SDL_MOUSEBUTTONUP:
            LOG_trace("Mouse button %d released.", (int)event.button.button);
            //if(event.button.button == SDL_BUTTON_LEFT)
            //{

            //}
            //else if(event.button.button == SDL_BUTTON_RIGHT)
            //{

            //}
            //else if(event.button.button == SDL_BUTTON_MIDDLE)
            //{

            //}
            break;

        case SDL_MOUSEMOTION:
#ifdef _DEBUG
            if(getCursorDown(SDL_BUTTON_RIGHT) && m_debugUI->particleEditor->open && m_debugUI->visible)
                m_debugUI->particleEditor->particles->emitFrom.centerOn(worldPosFromCursor(Vec2(event.motion.x, event.motion.y), Vec3(0.0f, 0.0f, m_fDefCameraZ)));
#endif
            //LOG_info("Mouse moved to ", event.motion.x, ", ", event.motion.y;
            break;
    }
}

#ifdef _DEBUG
static float curRate = 1.0f;
#define MAX_SLOW 0.0625f
#define SLOW 0.25f
#define MAX_FAST 3.0f
#define FAST 2.0f
#define INTERP_TIME 0.25f
#endif
void GameEngine::handleKeys()
{
#ifdef _DEBUG
    setTimeScale(curRate);
    if(getInterpolationManager()->contains(&curRate))
        return;

    float interpRate = 1.0f;
    if (getInputManager()->keyDown(SDL_SCANCODE_G))
    {
        if (getInputManager()->keyDown(SDL_SCANCODE_CTRL))
            interpRate = MAX_SLOW;
        else
            interpRate = SLOW;
    }

    else if (getInputManager()->keyDown(SDL_SCANCODE_H))
    {
        if (getInputManager()->keyDown(SDL_SCANCODE_CTRL))
            interpRate = MAX_FAST;
        else
            interpRate = FAST;
    }

    if(curRate != interpRate)
    {
        getInterpolationManager()->interpolate(&curRate, interpRate, INTERP_TIME);
    }

#endif
}




