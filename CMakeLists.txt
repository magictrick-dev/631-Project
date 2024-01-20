cmake_minimum_required(VERSION 3.21)

# --- Project Properties -------------------------------------------------------

project(j5)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
        VS_STARTUP_PROJECT ${PLATFORM_EXECUTABLE_NAME}) # why the hell is this not the default
    add_definitions(/MP) # Makes win32 MSVC compiler not as sucky slow
endif (WIN32)

# --- Project Sources ----------------------------------------------------------

add_executable(j5
    "./source/main.cpp"
    "./source/core.h"
    "./source/core/definitions.h"
    "./source/core/primitives.h"
    "./source/window.h"
    "./source/window.cpp"
)

target_include_directories(j5 PUBLIC
    "./source"
    "./vendor"
)
