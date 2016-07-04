set(engine_src
Mesh3D.cpp
Mesh3D.h
DebugDraw.cpp
Drawable.h
Engine.cpp
Engine.h
EngineContactListener.cpp
EngineContactListener.h
Gradient.cpp
Gradient.h
Image.cpp
Image.h
Node.cpp
Node.h
Object.cpp
Object.h
Text.cpp
Text.h
Arc.cpp
Arc.h
MouseCursor.cpp
MouseCursor.h
engine_gl.cpp
engine_sound.cpp
engine_window.cpp
globaldefs.cpp
globaldefs.h
HUD.cpp
HUD.h
Lattice.cpp
Lattice.h
luadefines.h
luafuncs.h
LuaInterface.cpp
LuaInterface.h
mtrand.cpp
mtrand.h
opengl-api.cpp
opengl-api.h
opengl-stubs.h
ParticleSystem.cpp
ParticleSystem.h
simplexnoise1234.cpp
simplexnoise1234.h
tiny3d.h
Interpolator.cpp
Interpolator.h
Random.cpp
Random.h
imgui_impl_opengl2.cpp
#imgui_impl_opengl3.cpp
imgui_impl_sdl.cpp
imgui_impl_sdl.h
MemoryEditor.cpp
MemoryEditor.h
ResourceLoader.cpp
ResourceLoader.h
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
