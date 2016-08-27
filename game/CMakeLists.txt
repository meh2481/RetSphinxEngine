set(game_src
GameEngine.cpp
GameEngine.h
GameEngine.rc
GameEngine_color.cpp
GameEngine_events.cpp
GameEngine_xmlparse.cpp
DebugUI.cpp
DebugUI.h
LuaFuncs.cpp
main.cpp
)

add_executable(game ${game_src})

set(EXTRA_LIBS "")

if(WIN32)
    set(EXTRA_LIBS ${EXTRA_LIBS} ws2_32)
endif()

target_link_libraries(game engine net ${EXTRA_LIBS})
