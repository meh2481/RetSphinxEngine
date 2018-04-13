#include "Engine.h"
#include "DebugDraw.h"
#include "Logger.h"
#include "ResourceLoader.h"
#include "Quad.h"
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include "VulkanInterface.h"

#define GAME_CONTROLLER_DB_FILE "gamecontrollerdb.txt"

void Engine::setup_sdl()
{

    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    {
        LOG(ERR) << "SDL_InitSubSystem Error: " << SDL_GetError();
        exit(1);
    }

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
        LOG(ERR) << "Unable to init SDL2 gamepad subsystem.";

    LOG(INFO) << "Loading Vulkan...";

    if (SDL_Vulkan_LoadLibrary(NULL) == -1)
    {
        LOG(ERR) << "SDL_Vulkan_LoadLibrary Error: " << SDL_GetError();
        exit(1);
    }

    // Quit SDL properly on exit
    atexit(SDL_Quit);

    // Create SDL window
    Uint32 flags = SDL_WINDOW_VULKAN;
    if(m_bResizable)
        flags |= SDL_WINDOW_RESIZABLE;

    m_Window = SDL_CreateWindow(m_sTitle.c_str(),
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             m_iWidth,
                             m_iHeight,
                             flags);

    if(m_Window == NULL)
    {
        LOG(ERR) << "Couldn't set video mode: " << SDL_GetError();
        exit(1);
    }

    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);
    if(!mode.refresh_rate)    //If 0, display doesn't care, so default to 60
        mode.refresh_rate = 60;
    LOG(DBG) << "Setting framerate to " << mode.refresh_rate << "Hz";
    setFramerate((float)mode.refresh_rate);

    int numDisplays = SDL_GetNumVideoDisplays();
    LOG(INFO) << "Available displays: " << numDisplays;
    for(int display = 0; display < numDisplays; display++)
    {
        int num = SDL_GetNumDisplayModes(display);
        LOG(TRACE) << "Available modes for display " << display + 1 << ':';
        for(int i = 0; i < num; i++)
        {
            SDL_GetDisplayMode(display, i, &mode);
            LOG(TRACE) << "Mode: " << mode.w << "x" << mode.h << " " << mode.refresh_rate << "Hz";
        }
    }

    LOG(INFO) << "Loading gamepad configurations from " << GAME_CONTROLLER_DB_FILE;
    if(SDL_GameControllerAddMappingsFromFile(GAME_CONTROLLER_DB_FILE) == -1)
        LOG(WARN) << "Unable to open " << GAME_CONTROLLER_DB_FILE << ": " << SDL_GetError();

    _loadicon();    //Load our window icon
}

//Set up Vulkan
void Engine::setup_vulkan()
{
    m_vulkan = new VulkanInterface(m_Window);

    glm::mat4 persp = glm::tweakedInfinitePerspective(glm::radians(45.0f), (float)m_iWidth / (float)m_iHeight, 0.1f);
    m_renderState.projection = persp;
}

void Engine::teardown_vulkan()
{
    delete m_vulkan;
    SDL_Vulkan_UnloadLibrary();
}
