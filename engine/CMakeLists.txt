set(engine_src
Arc.cpp
Arc.h
BezierInterpolation.h
BezierInterpolation.cpp
Color.cpp
Color.h
DebugDraw.h
DebugDraw.cpp
Engine.cpp
Engine.h
Engine_Vulkan.cpp
Engine_Window.cpp
EngineContactListener.cpp
EngineContactListener.h
Gradient.cpp
Gradient.h
HeadTracker.cpp
HeadTracker.h
ImgFont.cpp
ImgFont.h
imgui_impl_vulkan.cpp
imgui_impl_sdl.cpp
imgui_impl_sdl.h
InputDevice.cpp
InputDevice.h
Interpolation.h
LinearInterpolation.h
LinearInterpolation.cpp
LuaDefines.h
LuaFuncs.h
LuaInterface.cpp
LuaInterface.h
mtrand.cpp
mtrand.h
Node.cpp
Node.h
Object.cpp
Object.h
Object3D.cpp
Object3D.h
ObjSegment.h
ObjSegment.cpp
ParticleSystem.cpp
ParticleSystem.h
PointQueryCallback.h
Random.cpp
Random.h
Rect.cpp
Rect.h
RenderState.h
RenderState.cpp
ResourceLoader.cpp
ResourceLoader.h
ResourceCache.h
ResourceCache.cpp
SystemUtils.cpp
SystemUtils.h
Text.cpp
Text.h
VulkanInterface.cpp
VulkanInterface.h
Quad.h
Quad.cpp
)

set(engine_events_src
events/Observer.h
events/Subject.cpp
events/Subject.h
)

set(engine_manager_src
managers/ParticleSystemManager.h
managers/ParticleSystemManager.cpp
managers/EntityManager.h
managers/EntityManager.cpp
managers/InterpolationManager.h
managers/InterpolationManager.cpp
managers/NodeManager.cpp
managers/NodeManager.h
managers/ObjectManager.h
managers/ObjectManager.cpp
managers/SceneryManager.cpp
managers/SceneryManager.h
managers/Stringbank.cpp
managers/Stringbank.h
managers/InputManager.cpp
managers/InputManager.h
managers/SoundManager.h
managers/SoundManager.cpp
)

set(engine_actions_src
actions/Action.h
actions/ActionBind.h
actions/AxisAction.h
actions/AxisAction.cpp
actions/CombinedAction.h
actions/CombinedAction.cpp
actions/JoyButtonAction.h
actions/JoyButtonAction.cpp
actions/KeyboardAction.h
actions/KeyboardAction.cpp
actions/MouseButtonAction.h
actions/MouseButtonAction.cpp
actions/Movement.h
actions/MovementBind.h
actions/NoAction.h
actions/NoMovement.h
actions/JoystickMovement.h
actions/JoystickMovement.cpp
actions/KeyboardMovement.h
actions/KeyboardMovement.cpp
actions/DpadMovement.h
actions/DpadMovement.cpp
)

set(engine_sound_src
sound/SoundVol.h
sound/SoundVol.cpp
)

include_directories (.)
include_directories (events)
include_directories (sound)
include_directories (managers)
include_directories (actions)

source_group("events" FILES ${engine_events_src})
source_group("sound" FILES ${engine_sound_src})
source_group("managers" FILES ${engine_manager_src})
source_group("actions" FILES ${engine_actions_src})

add_library(engine ${engine_src} ${engine_events_src} ${engine_sound_src} ${engine_manager_src} ${engine_actions_src})
target_link_libraries(engine imgui lua53 io net ${BOX2D_LIBRARY} ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${FMOD_LIBRARIES} ${Vulkan_LIBRARIES})
