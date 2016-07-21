set(engine_src
Arc.cpp
Arc.h
Color.cpp
Color.h
DebugDraw.cpp
Drawable.h
Engine.cpp
Engine.h
engine_gl.cpp
engine_sound.cpp
engine_window.cpp
EngineContactListener.cpp
EngineContactListener.h
Font.cpp
Font.h
Gradient.cpp
Gradient.h
HUD.cpp
HUD.h
Image.cpp
Image.h
imgui_impl_opengl2.cpp
#imgui_impl_opengl3.cpp
imgui_impl_sdl.cpp
imgui_impl_sdl.h
Interpolator.cpp
Interpolator.h
Lattice.cpp
Lattice.h
luadefines.h
luafuncs.h
LuaInterface.cpp
LuaInterface.h
MemoryEditor.cpp
MemoryEditor.h
Mesh3D.cpp
Mesh3D.h
MouseCursor.cpp
MouseCursor.h
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
Random.cpp
Random.h
Rect.cpp
Rect.h
ResourceLoader.cpp
ResourceLoader.h
ResourceCache.h
ResourceCache.cpp
simplexnoise1234.cpp
simplexnoise1234.h
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
)

include_directories (.)
include_directories (events)
include_directories (managers)

source_group("events" FILES ${engine_events_src})
source_group("managers" FILES ${engine_manager_src})

add_library(engine ${engine_src} ${engine_events_src} ${engine_manager_src})
target_link_libraries(engine imgui lua53 io ${BOX2D_LIBRARY} ${SDL2_LIBRARY})
