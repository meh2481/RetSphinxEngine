set(game_src
DebugUI.cpp
DebugUI.h
GameEngine.cpp
GameEngine.h
GameEngine.rc
GameEngine_color.cpp
GameEngine_events.cpp
GameEngine_xmlparse.cpp
LuaFuncs.cpp
main.cpp
MemoryEditor.cpp
MemoryEditor.h
ParticleEditor.cpp
ParticleEditor.h
SteelSeriesEditor.h
SteelSeriesEditor.cpp
)

add_executable(game ${game_src})

set(EXTRA_LIBS "")

if(WIN32)
    set(EXTRA_LIBS ${EXTRA_LIBS} ws2_32)
endif()

target_link_libraries(game engine net ${EXTRA_LIBS})
