#include "Engine.h"
#include "DebugDraw.h"
#include "Logger.h"
#include "ResourceLoader.h"
#include "Quad.h"
#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>
#include "VulkanInterface.h"

#define GAME_CONTROLLER_DB_FILE "gamecontrollerdb.txt"

//HACK
VulkanInterface* g_vulkan;

void Engine::setup_sdl()
{

    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    {
        LOG_err("SDL_InitSubSystem Error: %s", SDL_GetError());
        exit(1);
    }

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
        LOG_err("Unable to init SDL2 gamepad subsystem.");

    LOG_info("Loading Vulkan...");

    if (SDL_Vulkan_LoadLibrary(NULL) == -1)
    {
        LOG_err("SDL_Vulkan_LoadLibrary Error: %s", SDL_GetError());
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
        LOG_err("Couldn't set video mode: %s", SDL_GetError());
        exit(1);
    }

    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);
    if(!mode.refresh_rate)    //If 0, display doesn't care, so default to 60
        mode.refresh_rate = 60;
    LOG_dbg("Setting framerate to %dHz", mode.refresh_rate);
    setFramerate((float)mode.refresh_rate);

    int numDisplays = SDL_GetNumVideoDisplays();
    LOG_info("Available displays: %d", numDisplays);
    for(int display = 0; display < numDisplays; display++)
    {
        int num = SDL_GetNumDisplayModes(display);
        LOG_info("Available modes for display %d", display + 1);
        for(int i = 0; i < num; i++)
        {
            SDL_GetDisplayMode(display, i, &mode);
            LOG_info("Mode: %dx%d %dHz", mode.w, mode.h, mode.refresh_rate);
        }
    }

    LOG_info("Loading gamepad configurations from %s", GAME_CONTROLLER_DB_FILE);
    if(SDL_GameControllerAddMappingsFromFile(GAME_CONTROLLER_DB_FILE) == -1)
        LOG_warn("Unable to open %s: %s", GAME_CONTROLLER_DB_FILE, SDL_GetError());

    _loadicon();    //Load our window icon
}

//Set up Vulkan
void Engine::setup_vulkan()
{
    m_vulkan = new VulkanInterface(m_Window, m_resourceLoader);
    g_vulkan = m_vulkan;

    glm::mat4 persp = glm::tweakedInfinitePerspective(glm::radians(45.0f), (float)m_iWidth / (float)m_iHeight, 0.1f);
    m_renderState.proj = persp;
}

void Engine::teardown_vulkan()
{
    delete m_vulkan;
    SDL_Vulkan_UnloadLibrary();
}
