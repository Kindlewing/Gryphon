cmake_minimum_required(VERSION 3.20)
project(gryphon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------------
# Base library
# -------------------------------
set(BASE_SOURCES
    src/base/base_math.c
    src/base/profile.c
    src/base/string8.c
)

if(WIN32)
    list(APPEND BASE_SOURCES src/base/arena_win32.c)
elseif(UNIX AND NOT APPLE)
    list(APPEND BASE_SOURCES src/base/arena_linux.c)
else()
    message(FATAL_ERROR "Unsupported platform for base library")
endif()

add_library(base STATIC ${BASE_SOURCES})

target_include_directories(base
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(base PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    $<$<C_COMPILER_ID:MSVC>:/W4>
)

# -------------------------------
# Gryphon library
# -------------------------------
# Core + render + GLAD
set(GRYPHON_SOURCES
    src/gryphon/gryphon.c
    src/gryphon/render/shader.c
    third_party/glad/src/glad.c
)

# Platform-specific sources
if(WIN32)
    list(APPEND GRYPHON_SOURCES src/gryphon/platform/win32.c)
elseif(UNIX AND NOT APPLE)
    list(APPEND GRYPHON_SOURCES src/gryphon/platform/x11.c)
    find_package(X11 REQUIRED)
else()
    message(FATAL_ERROR "Gryphon currently supports only Windows and Linux (X11).")
endif()

find_package(OpenGL REQUIRED)

add_library(gryphon STATIC ${GRYPHON_SOURCES})

# Public includes for consumers
target_include_directories(gryphon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
)

target_link_libraries(gryphon
    PUBLIC
        base
        OpenGL::GL
        $<$<BOOL:${X11_FOUND}>:${X11_LIBRARIES}>
    PRIVATE
        dl  # for GLAD dynamic loading on Linux
)

target_compile_options(gryphon PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    $<$<C_COMPILER_ID:MSVC>:/W4>
)

# -------------------------------
# In-tree test binary
# -------------------------------
option(BUILD_TEST_APP "Build Gryphon test binary" ON)

if(BUILD_TEST_APP AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.c")
    add_executable(gry_test tests/main.c)
    
    # Link with gryphon (which already links base + OpenGL/X11)
    target_link_libraries(gry_test PRIVATE gryphon)
    
    # Only add include root — gryphon target already provides headers via PUBLIC
    target_include_directories(gry_test PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
endif()
