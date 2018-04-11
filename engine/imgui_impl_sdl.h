// ImGui SDL2 binding with Vulkan
// You can copy and use unmodified imgui_impl_* files in your project.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// See main.cpp for an example of using this.
// https://github.com/ocornut/imgui
#pragma once
#include "imgui/imgui.h"

struct SDL_Window;
typedef union SDL_Event SDL_Event;

IMGUI_API bool        ImGui_ImplSdl_Init(SDL_Window *window, const char* cIniFile);
IMGUI_API void        ImGui_ImplSdl_NewFrame(SDL_Window *window);
IMGUI_API bool        ImGui_ImplSdl_ProcessEvent(SDL_Event* event);

IMGUI_API void        ImGui_Impl_Vulkan_Shutdown();
IMGUI_API void        ImGui_Impl_Vulkan_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_Impl_Vulkan_CreateDeviceObjects();
IMGUI_API void        ImGui_Impl_Vulkan_RenderDrawLists(ImDrawData*);

IMGUI_API void        ImGui_Impl_Vulkan_DeleteContext(const void *ctx);
IMGUI_API void        ImGui_Impl_Vulkan_SwitchContext(const void *ctx);


extern unsigned g_imgui_FontTexture;
