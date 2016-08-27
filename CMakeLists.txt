cmake_minimum_required(VERSION 2.6)
project(retsphinx)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(PROJ_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Box2D REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# HACK: apparently not set automatically?
set(glm_INCLUDE_DIRS "${glm_DIR}/../" CACHE STRING "Path to glm includes")


include_directories(dep)
add_subdirectory(dep)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(io)
add_subdirectory(io)

include_directories("${glm_INCLUDE_DIRS}")
include_directories("${BOX2D_INCLUDE_DIRS}/..")
include_directories("${SDL2_INCLUDE_DIR}")
include_directories(dep/lua)
include_directories(dep/imgui)

add_subdirectory(net)
add_subdirectory(engine)

include_directories(net)
include_directories(engine)
include_directories(engine/events)
include_directories(engine/managers)
add_subdirectory(game)
add_subdirectory(tools)
