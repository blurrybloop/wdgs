cmake_minimum_required(VERSION 3.14)
project(wdgs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "./;${CMAKE_INSTALL_PREFIX}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include_directories(ext)
include_directories(include)

add_executable(wdgs
        WIN32
        src/application.cpp
        src/camera.cpp
        src/config.cpp
        src/debug.cpp
        ext/glad/glad.cpp
        src/gravity.cpp
        src/memmng.cpp
        src/mesh.cpp
        src/pch.cpp
        src/resources.cpp
        src/simrenderer.cpp
        src/simulation.cpp
        src/wdgs.cpp)

include(BuildAntTweakBar)
include(BuildGLFW)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()

target_link_libraries(${PROJECT_NAME} anttweakbar glfw)