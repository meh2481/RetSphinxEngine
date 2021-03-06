/*
    GameEngine header - Engine.h
    Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "SDL.h"
#include "RenderState.h"
#include "Rect.h"
#include <vector>

class b2World;
class Image;
class ResourceLoader;
class EntityManager;
class Stringbank;
class InputManager;
class SoundManager;
class InterpolationManager;
class HeadTracker;
class EngineContactListener;
class DebugDraw;

#define VELOCITY_ITERATIONS 8
#define PHYSICS_ITERATIONS 3

typedef struct
{
    std::string sSwitch, sValue;
} commandlineArg;

class Engine
{
private:
    //Variables for use by the engine
    std::string m_sTitle;
    std::string m_sAppName;
    std::string m_sCompanyName;
    std::string m_sIcon;
    SDL_Window* m_Window;
    std::vector<commandlineArg> lCommandLine;
    b2World* m_physicsWorld;
    EngineContactListener* m_clContactListener;
#ifdef _DEBUG
    DebugDraw* m_debugDraw;
    bool m_bDebugDraw;
#endif
    Vec2 m_ptCursorPos;
    bool  m_bShowCursor;
    float m_fFramerate;
    float m_fAccumulatedTime;
    float m_fTargetTime;

    bool m_bQuitting;   //Stop the game if this turns true
    float m_fTimeScale;    //So we can scale time if we want
    int m_iWidth, m_iHeight;
    int m_iNumScreenModes;      //Number of screen modes that are available
    bool m_bFullscreen;
    bool m_bResizable;
    float m_fGamma;        //Overall screen brightness
    bool m_bPaused;            //If the game is paused due to not being focused
    bool m_bControllerDisconnected;    //If the game is paused due to a controller disconnecting
    bool m_bPauseOnKeyboardFocus;    //If the game pauses when keyboard focus is lost
    int m_iMSAA;        //Antialiasing (0x, 2x, 4x, 8x, etc)
    bool m_bCursorShow;
    bool m_bCursorOutOfWindow;    //If the cursor is outside of the window, don't draw it
#ifdef _DEBUG
    bool m_bStepFrame;
    bool m_bSteppingPhysics;
#endif

    //OpenGL stuff
    RenderState m_renderState;
#ifdef _DEBUG
    RenderState m_debugRenderState;
#endif

    //Managers
    ResourceLoader* m_resourceLoader;
    EntityManager* m_entityManager;
    Stringbank* m_stringBank;
    InputManager* m_inputManager;
    SoundManager* m_soundManager;
    InterpolationManager* m_interpolationManager;

    //Engine-use function definitions
    bool _frame();
    void _render();

    void setup_sdl();
    void setup_opengl();
    void _loadicon();                    //Load icon and set window to have said icon

    bool _processEvent(SDL_Event& e);    //Engine-specific handling of events

    Engine() {}; //Default constructor isn't callable

protected:

    //Functions to override in your own class definition
    virtual void frame(float dt) = 0;   //Function that's called every frame
    virtual void draw(RenderState& renderState) = 0;    //Actual function that draws stuff
    virtual bool init(std::vector<commandlineArg> sArgs) = 0;    //So we can load all our images and such
    virtual void handleEvent(SDL_Event event) = 0;  //Function that's called for each SDL input event
    virtual void pause() = 0;    //Called when the window is deactivated
    virtual void resume() = 0;    //Called when the window is activated again
    virtual bool drawDebugUI() = 0;    //Test to see if debug (imGui) should be drawn

public:
    //Constructor/destructor
    Engine(uint16_t iWidth, uint16_t iHeight, const std::string& sTitle, const std::string& sCompanyName, const std::string& sAppName, const std::string& sIcon, bool bResizable = false);
    ~Engine();

    //Misc. methods
    void commandline(std::vector<std::string> argv);    //Pass along commandline arguments for the engine to use
    void start();   //Runs engine and doesn't exit until the engine ends
    void quit() { m_bQuitting = true; };  //Stop the engine and quit nicely
    std::string getSaveLocation();        //Get good location to save config files/save files
    Rect getCameraView(Vec3 Camera);        //Return the rectangle, in world position z=0, that the camera can see
    Vec2 worldPosFromCursor(Vec2 cursorpos, Vec3 Camera);    //Get the worldspace position of the given mouse cursor position
    Vec2 worldMovement(Vec2 cursormove, Vec3 Camera);        //Get the worldspace transform of the given mouse transformation
    std::string getAppName() { return m_sAppName; };        //Get the application name this engine was created with
    std::string getCompanyName() { return m_sCompanyName; };        //Get the company name this engine was created with

    //Drawing functions
    Rect getScreenRect() { Rect rc(0, 0, getWidth(), getHeight()); return rc; };
    void drawDebug();

    //Window functions - engine_window.cpp
    void changeScreenResolution(int w, int h);  //Change resolution mid-game and reload OpenGL textures as needed
    //void toggleFullscreen();                            //Switch between fullscreen/windowed modes
    void setFullscreen(bool bFullscreen);                //Set fullscreen to true or false as needed
    void setInitialFullscreen() { SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP); };
    bool isFullscreen() { return m_bFullscreen; };
    bool isMaximized();
    Vec2 getWindowPos();    //Get the window position
    void setWindowPos(Vec2 pos);    //Set window position
    void maximizeWindow();                                //Call window manager to maximize application window
    void pauseOnKeyboard(bool p) { m_bPauseOnKeyboardFocus = p; };
    bool pausesOnFocusLost() { return m_bPauseOnKeyboardFocus; };
    uint16_t getWidth() { return m_iWidth; };
    uint16_t getHeight() { return m_iHeight; };

    //Physics functions
    void setGravity(Vec2 ptGravity);
    void setGravity(float x, float y);
    void stepPhysics(float dt);    //Update our physics world and handle collisions
    b2World* getWorld() { return m_physicsWorld; };
#ifdef _DEBUG
    void setDebugDraw(bool b) { m_bDebugDraw = b; };
    bool getDebugDraw() { return m_bDebugDraw; };
    void toggleDebugDraw() { m_bDebugDraw = !m_bDebugDraw; };
    void playPausePhysics() { m_bSteppingPhysics = !m_bSteppingPhysics; };
    void pausePhysics() { m_bSteppingPhysics = true; };
    void playPhysics() { m_bSteppingPhysics = false; };
    void stepPhysics() { m_bStepFrame = true; };
#endif

    //Mouse functions
    Vec2 getCursorPos() { int x, y; SDL_GetMouseState(&x, &y); return(Vec2(x, y)); };
    void setCursorPos(Vec2 pos) { SDL_WarpMouseInWindow(m_Window, (int)pos.x, (int)pos.y); };
    bool getCursorDown(int iButtonCode);
    void showCursor() { SDL_ShowCursor(1); };
    void hideCursor() { SDL_ShowCursor(0); };
    void setCursor(SDL_Cursor* cur) { SDL_SetCursor(cur); };
    bool isMouseGrabbed();
    void grabMouse(bool bGrab = true);

    //Controller functions
    InputManager* getInputManager() { return m_inputManager;  };    //Get current input manager
    bool isControllerDisconnected() { return m_bControllerDisconnected; }

    //Time functions
    float getTimeScale() { return m_fTimeScale; };
    void setTimeScale(float fScale);
    unsigned getTicks();
    float getSeconds();
    void setFramerate(float fFramerate);
    float getFramerate() { return m_fFramerate; };

    //OpenGL methods
    void setDoubleBuffered(bool bDoubleBuffered);
    bool getDoubleBuffered();
    void setVsync(int iVsync);
    int getVsync();
    int getMSAA() { return m_iMSAA; };
    void setMSAA(int iMSAA);
    void setGamma(float fGamma) { m_fGamma = fGamma; };
    float getGamma() { return m_fGamma; };

    //---------------------------------------------------------
    // Managers
    EntityManager* getEntityManager() { return m_entityManager; };
    ResourceLoader* getResourceLoader() { return m_resourceLoader; };
    Stringbank* getStringbank() { return m_stringBank; };
    SoundManager* getSoundManager() { return m_soundManager; }
    InterpolationManager* getInterpolationManager() { return m_interpolationManager; }
};
