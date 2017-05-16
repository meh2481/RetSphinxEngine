set(engine_src
Arc.cpp
Arc.h
Color.cpp
Color.h
DebugDraw.cpp
DrawableItem.h
Engine.cpp
Engine.h
Engine_GL.cpp
Engine_Window.cpp
EngineContactListener.cpp
EngineContactListener.h
ImgFont.cpp
ImgFont.h
Gradient.cpp
Gradient.h
Image.cpp
Image.h
imgui_impl_opengl2.cpp
#imgui_impl_opengl3.cpp
imgui_impl_sdl.cpp
imgui_impl_sdl.h
InputDevice.cpp
InputDevice.h
Lattice.cpp
Lattice.h
LuaDefines.h
LuaFuncs.h
LuaInterface.cpp
LuaInterface.h
Mesh3D.cpp
Mesh3D.h
mtrand.cpp
mtrand.h
Node.cpp
Node.h
Object.cpp
Object.h
opengl-api.cpp
opengl-api.h
opengl-stubs.h
OpenGLShader.cpp
OpenGLShader.h
ParticleSystem.cpp
ParticleSystem.h
PointQueryCallback.h
Random.cpp
Random.h
Rect.cpp
Rect.h
ResourceLoader.cpp
ResourceLoader.h
ResourceCache.h
ResourceCache.cpp
SystemUtils.cpp
SystemUtils.h
tiny3d.h
Text.cpp
Text.h
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
actions/NoAction.h
actions/AxisAction.h
actions/AxisAction.cpp
actions/JoyButtonAction.h
actions/JoyButtonAction.cpp
actions/KeyboardAction.h
actions/KeyboardAction.cpp
actions/MouseButtonAction.h
actions/MouseButtonAction.cpp
actions/Movement.h
actions/MovementBind.h
actions/NoMovement.h
actions/JoystickMovement.h
actions/JoystickMovement.cpp
actions/KeyboardMovement.h
actions/KeyboardMovement.cpp
actions/DpadMovement.h
actions/DpadMovement.cpp
)

include_directories (.)
include_directories (events)
include_directories (managers)
include_directories (actions)

source_group("events" FILES ${engine_events_src})
source_group("managers" FILES ${engine_manager_src})
source_group("actions" FILES ${engine_actions_src})

add_library(engine ${engine_src} ${engine_events_src} ${engine_manager_src} ${engine_actions_src})
target_link_libraries(engine imgui lua53 io net ${BOX2D_LIBRARY} ${SDL2_LIBRARY} ${FMOD_LIBRARIES})
