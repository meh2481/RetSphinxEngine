/*
    GameEngine source - Engine.cpp
    Copyright (c) 2014 Mark Hutcheson
*/
#include <SDL_syswm.h>
#include "Engine.h"
#include "Box2D/Box2D.h"
#include "Logger.h"
#include "Random.h"
#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "Stringbank.h"
#include "stb_image.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "InterpolationManager.h"
#include "EngineContactListener.h"
#include "DebugDraw.h"
#include "Quad.h"
#include "VulkanInterface.h"

#define GUID_STR_SZ    256
#define STRINGBANK_LOCATION "res/stringbank.xml"
#define PAK_LOCATION "res/pak"
#define IMGUI_INI "imgui.ini"
#define LOGGING_CONF "logging.conf"
#define LOGFILE_NAME "logfile.log"

Engine::Engine(uint16_t iWidth, uint16_t iHeight, const std::string& sTitle, const std::string& sCompanyName, const std::string& sAppName, const std::string& sIcon, bool bResizable)
{
    m_sTitle = sTitle;
    m_sAppName = sAppName;
    m_sCompanyName = sCompanyName;

    //Start logger
    logger_init((getSaveLocation() + LOGFILE_NAME).c_str(), DBG);

    m_sIcon = sIcon;
    m_bResizable = bResizable;

    LOG(INFO) << "-----------------------BEGIN PROGRAM EXECUTION-----------------------";
#ifdef _DEBUG
    LOG(INFO) << "Debug build";
    m_bDebugDraw = true;
    m_bSoundDebugDraw = false;
#else
    LOG(INFO) << "Release build";
#endif
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_iMSAA = 0;    //Default: MSAA off
    m_bShowCursor = true;
    m_fFramerate = 60.0f;
    setFramerate(60);     //60 fps default
    m_bFullscreen = true;

    m_fGamma = 1.0f;
    m_bPaused = m_bControllerDisconnected = false;
    m_bPauseOnKeyboardFocus = true;
    m_bCursorShow = true;
    m_bCursorOutOfWindow = false;
#ifdef _DEBUG
    m_bSteppingPhysics = false;
    m_bStepFrame = false;
#endif

    //Initialize engine stuff
    m_bQuitting = false;
    Random::seed(SDL_GetTicks());
    m_fTimeScale = 1.0f;

    LOG(DBG) << "Create physics world";
    b2Vec2 gravity(0.0f, -9.8f);    //Vector for our world's gravity
    m_physicsWorld = new b2World(gravity);
    m_physicsWorld->SetAllowSleeping(true);
    m_clContactListener = new EngineContactListener();
    m_physicsWorld->SetContactListener(m_clContactListener);

    LOG(DBG) << "Creating resource loader";
    m_resourceLoader = new ResourceLoader(m_physicsWorld, PAK_LOCATION);
    m_entityManager = new EntityManager(m_resourceLoader, m_physicsWorld);
    m_stringBank = m_resourceLoader->getStringbank(STRINGBANK_LOCATION);

    m_inputManager = new InputManager();
    m_interpolationManager = new InterpolationManager();

    //Init sound manager
    m_soundManager = new SoundManager(m_resourceLoader, m_interpolationManager);

    //This needs to be in memory when ImGUI goes to load/save INI settings, so it's static
    static const std::string sIniFile = getSaveLocation() + IMGUI_INI;

    //Init renderer
    LOG(DBG) << "Init SDL/Vulkan";
    setup_sdl();
    setup_vulkan();

#ifdef _DEBUG
    LOG(DBG) << "Create debug draw";
    m_debugDraw = new DebugDraw(m_debugRenderState);
    m_debugDraw->outlineAlpha = 0.75f;
    m_debugDraw->fillAlpha = 0.5f;
    m_debugDraw->fillMul = m_debugDraw->fillAlpha;
    m_debugDraw->SetFlags(DebugDraw::e_shapeBit | DebugDraw::e_jointBit);
    m_physicsWorld->SetDebugDraw(m_debugDraw);
#endif

    //Init ImGUI
    ImGui_ImplSdl_Init(m_Window, sIniFile.c_str());

    //TODO
    void *glctx = NULL;// SDL_GL_GetCurrentContext();
    ImGui_Impl_Vulkan_SwitchContext(glctx);
    ImGui_Impl_Vulkan_CreateDeviceObjects();
}

Engine::~Engine()
{
    //Clean up managers
    delete m_entityManager;
    delete m_resourceLoader;
    delete m_inputManager;
    delete m_soundManager;
    delete m_interpolationManager;
    delete m_clContactListener;
#ifdef _DEBUG
    delete m_debugDraw;
#endif
    Draw::shutdown();

    //Clean up ImGui
    ImGui_Impl_Vulkan_Shutdown();

    //Clean up vulkan
    teardown_vulkan();

    //Clean up SDL
    SDL_DestroyWindow(m_Window);

    // Clean up and shutdown
    LOG(INFO) << "Deleting phys world";
    delete m_physicsWorld;
    LOG(INFO) << "Quit SDL";
    SDL_Quit();
}

void Engine::start()
{
    // Load all that we need to
    if(init(lCommandLine))
        while(!_frame());
}

bool Engine::_processEvent(SDL_Event& e)
{
    //Engine-specific event processing
    switch(e.type)
    {
        case SDL_MOUSEMOTION:
            m_ptCursorPos.x = (float)e.motion.x;
            m_ptCursorPos.y = (float)e.motion.y;
            break;

        case SDL_WINDOWEVENT:
            if(e.window.event == SDL_WINDOWEVENT_FOCUS_LOST && m_bPauseOnKeyboardFocus)
            {
                m_bPaused = true;
                pause();
            }
            else if(e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED && m_bPauseOnKeyboardFocus)
            {
                m_bPaused = false;
                if(!m_bControllerDisconnected)
                    resume();
            }
            else if(e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                if(m_bResizable)
                    changeScreenResolution(e.window.data1, e.window.data2);
                else
                    LOG(ERR) << "Resize event generated, but resizable flag not set.";
            }
            else if(e.window.event == SDL_WINDOWEVENT_ENTER)
                m_bCursorOutOfWindow = false;
            else if(e.window.event == SDL_WINDOWEVENT_LEAVE)
                m_bCursorOutOfWindow = true;
            break;

        case SDL_QUIT:
            return true;

        case SDL_CONTROLLERDEVICEADDED:
            m_inputManager->addController(e.cdevice.which);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            LOG(INFO) << "Controller " << (int)e.cdevice.which << " disconnected.";
            m_inputManager->removeController(e.cdevice.which);
            break;

        case SDL_JOYDEVICEADDED:
        {
            SDL_Joystick* joy = SDL_JoystickOpen(e.jdevice.which);
            if(joy)
            {
                char guid[GUID_STR_SZ];
                SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, GUID_STR_SZ);

                LOG(INFO) << "Joystick " << SDL_JoystickName(joy) << " attached. Not using joystick API, but here's info:";
                LOG(INFO) << "Joystick has GUID " << guid;
                LOG(INFO) << "Joystick Number of Axes: " << SDL_JoystickNumAxes(joy);
                LOG(INFO) << "Joystick Number of Buttons: " << SDL_JoystickNumButtons(joy);
                LOG(INFO) << "Joystick Number of Balls: " << SDL_JoystickNumBalls(joy);
                LOG(INFO) << "Joystick Number of Hats: " << SDL_JoystickNumHats(joy);
                if(SDL_JoystickNumAxes(joy) == 3)    //Head tracker
                    m_inputManager->addHeadTracker(joy);
                else
                    SDL_JoystickClose(joy);
            }
            break;
        }

        case SDL_JOYDEVICEREMOVED:
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYHATMOTION:
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_KEYDOWN:
            m_inputManager->activateController(-1);    //Mouse+kb control
            //Fall through
        case SDL_CONTROLLERBUTTONDOWN:
            //Unpause when selecting with a new input device
            if(m_bControllerDisconnected)
            {
                resume();    //Unpause game
                m_bControllerDisconnected = false;
            }
            break;
    }

    return false;
}

bool Engine::_frame()
{
    Vec2 pos(0, 0);
    Vec2 vel(0, 0);
    m_soundManager->update();

    //Handle input events from SDL
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(drawDebugUI())
            ImGui_ImplSdl_ProcessEvent(&event);

        if(_processEvent(event))
            return true;

        //Pass on to derived game engine class
        if(!m_bPaused && !m_bControllerDisconnected)
            handleEvent(event);
    }

    if(drawDebugUI())
        ImGui_ImplSdl_NewFrame(m_Window);

#ifdef _DEBUG
    if(!m_bSteppingPhysics || m_bStepFrame)
    {
        m_bStepFrame = false;
#endif
        if(!m_bPaused && !m_bControllerDisconnected)
            frame(m_fTargetTime);    //Box2D wants fixed timestep, so we use target framerate here instead of actual elapsed time

#ifdef _DEBUG
    }
#endif

    _render();    //Vulkan handles our framerate sync

    return m_bQuitting;
}

void Engine::_render()
{
    // Game-specific drawing
    draw(m_renderState);

#ifdef _DEBUG
    drawDebug();

    if(drawDebugUI())
        ImGui::Render();
#endif
    Draw::flush();

    m_vulkan->mainLoop();
}


void Engine::drawDebug()
{
#ifdef _DEBUG
    // Draw physics debug stuff
    if(m_bDebugDraw)
        m_physicsWorld->DrawDebugData();
    if(m_bSoundDebugDraw)
        getSoundManager()->drawDebug(m_debugDraw);
#endif // _DEBUG
}

void Engine::setFramerate(float fFramerate)
{
    LOG(TRACE) << "Setting framerate to " << fFramerate;
    if(fFramerate < 60.0)
        fFramerate = 60.0;    //60fps is bare minimum
    m_fFramerate = fFramerate;
    m_fTargetTime = 1.0f / m_fFramerate;
}

void Engine::setMSAA(int iMSAA)
{
    m_iMSAA = iMSAA;
    //TODO
}

void Engine::_loadicon()    //Load icon into SDL window
{
    LOG(INFO) << "Load icon " << m_sIcon;

    SDL_Surface *surface = getResourceLoader()->getSDLImage(m_sIcon);
    if(surface == NULL)
        LOG(ERR) << "NULL surface for icon " << m_sIcon;
    else
    {
        SDL_SetWindowIcon(m_Window, surface);
        SDL_FreeSurface(surface);
    }
}

bool Engine::getCursorDown(int iButtonCode)
{
    return (!!SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(iButtonCode));
}

void Engine::commandline(std::vector<std::string> argv)
{
    //Step through intelligently
    for(std::vector<std::string>::iterator i = argv.begin(); i != argv.end(); i++)
    {
        commandlineArg cla;
        std::string sSwitch = *i;
        if(sSwitch.find('-') == 0)
        {
            if(sSwitch.find("--") == 0)
                sSwitch.erase(0, 1);
            sSwitch.erase(0, 1);

            cla.sSwitch = sSwitch;
            std::vector<std::string>::iterator sw = i;
            if(++sw != argv.end())    //Switch with a value
            {
                cla.sValue = *sw;
                i++;
                if(i == argv.end()) break;
            }
        }
        else    //No switch for this value
            cla.sValue = sSwitch;
        lCommandLine.push_back(cla);
    }
}

std::string Engine::getSaveLocation()
{
    char* cPath = SDL_GetPrefPath(m_sCompanyName.c_str(), m_sAppName.c_str());
    std::string s;
    if(cPath)
        s = cPath;
    else
        s = "./";
    SDL_free(cPath);
    return s;
}

Rect Engine::getCameraView(Vec3 Camera)
{
    Rect rcCamera;
    const float tan45_2 = 0.4142135623f;// tan(glm::radians(45.0f / 2.0f));
    const float fAspect = (float)getWidth() / (float)getHeight();
    rcCamera.bottom = (tan45_2 * Camera.z);
    rcCamera.top = -(tan45_2 * Camera.z);
    rcCamera.left = rcCamera.bottom * fAspect;
    rcCamera.right = rcCamera.top * fAspect;
    rcCamera.offset(-Camera.x, -Camera.y);
    return rcCamera;
}

Vec2 Engine::worldMovement(Vec2 cursormove, Vec3 Camera)
{
    cursormove.x /= (float)getWidth();
    cursormove.y /= (float)getHeight();

    Rect rcCamera = getCameraView(Camera);
    cursormove.x *= rcCamera.width();
    cursormove.y *= rcCamera.height();

    return cursormove;
}

Vec2 Engine::worldPosFromCursor(Vec2 cursorpos, Vec3 Camera)
{
    //Rectangle that the camera can see in world space
    Rect rcCamera = getCameraView(Camera);

    //Our relative position in window rect space (in rage 0-1)
    cursorpos.x /= (float)getWidth();
    cursorpos.y /= -(float)getHeight();    //Flip on y

    //Multiply this by the size of the world rect to get the relative cursor pos
    cursorpos.x = cursorpos.x * rcCamera.width() + rcCamera.left;
    cursorpos.y = cursorpos.y * rcCamera.height() + rcCamera.top;

    return cursorpos;
}

void Engine::stepPhysics(float dt)
{
    m_physicsWorld->Step(dt, VELOCITY_ITERATIONS, PHYSICS_ITERATIONS);

    //Update collisions
    std::set<b2Contact*> contacts = m_clContactListener->currentContacts;    //Grab all the currently-active contacts
    //Set join the short contacts that fired and also quit this frame
    for(std::set<b2Contact*>::iterator i = m_clContactListener->frameContacts.begin(); i != m_clContactListener->frameContacts.end(); i++)
        contacts.insert(*i);

    //Iterate over all these
    for(std::set<b2Contact*>::iterator i = contacts.begin(); i != contacts.end(); i++)
    {
        Collision c = m_clContactListener->getCollision(*i);
        b2WorldManifold worldManifold;
        (*i)->GetWorldManifold(&worldManifold);
        if(c.objA && c.objB)
        {
            //Let both scripts handle colliding, for more generic collision in both
            c.objA->collide(c.objB, c.impulse);
            c.objB->collide(c.objA, c.impulse);
        }
        else if(c.objA && !c.nodeB)
        {
            b2Vec2 pt = -worldManifold.normal;    //Flip this, since a Box2D normal is defined from A->B, and we want a wall->obj normal
            c.objA->collideWall(Vec2(pt.x, pt.y), c.impulse);
        }
        else if(c.objB && !c.nodeA)
        {
            c.objB->collideWall(Vec2(worldManifold.normal.x, worldManifold.normal.y), c.impulse);
        }
        //Don't care about two non-object fixtures colliding

        //Test for objects entering nodes (sensors)
        if(c.nodeA && c.objB)
        {
            c.nodeA->collided(c.objB);
        }
        else if(c.nodeB && c.objA)
        {
            c.nodeB->collided(c.objA);
        }
    }
    m_clContactListener->clearFrameContacts();    //Erase all these now that we've handled them
}

void Engine::setDoubleBuffered(bool bDoubleBuffered)
{
    //TODO
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, bDoubleBuffered);
}

bool Engine::getDoubleBuffered()
{
    int val = 1;
    //TODO
    //SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &val);
    return !!val;
}

void Engine::setVsync(int iVsync)
{
    //TODO
    //if(SDL_GL_SetSwapInterval(iVsync) == -1)
    //    SDL_GL_SetSwapInterval(1); //Default to vsync on if this fails
}

int Engine::getVsync()
{
    //TODO
    return 1;// SDL_GL_GetSwapInterval();
}

void Engine::setGravity(Vec2 ptGravity)
{
    m_physicsWorld->SetGravity(b2Vec2(ptGravity.x, ptGravity.y));
}

void Engine::setGravity(float x, float y)
{
    setGravity(Vec2(x, y));
}

bool Engine::isMouseGrabbed()
{
    return !!SDL_GetWindowGrab(m_Window);
}

void Engine::grabMouse(bool bGrab)
{
    SDL_SetWindowGrab(m_Window, (SDL_bool)bGrab);
}

unsigned Engine::getTicks()
{
    return SDL_GetTicks();
}

float Engine::getSeconds()
{
    return (float)SDL_GetTicks() / 1000.0f;
}

void Engine::setTimeScale(float fScale)
{
    m_fTimeScale = fScale;
    m_soundManager->setPlaybackRate(fScale);
}
