cmake_minimum_required(VERSION 3.24)
project(Gryphon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(Gryphon STATIC)
add_library(Glad STATIC)


if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

option(DEV "Build the dev project" OFF)
option(PROFILE_ENABLE "Enable profiling" OFF)

find_package(OpenGL REQUIRED)

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
endif()

# Gryphon sources
target_sources(Gryphon
	PRIVATE
		src/gryphon.c
		src/platform/x11.c
		src/render/shader.c
		src/render/renderer.c
)
# Glad sources
target_sources(Glad
	PRIVATE
		third_party/glad/src/glad.c
)


target_include_directories(Gryphon PUBLIC include PRIVATE src)
target_include_directories(Glad PRIVATE third_party/glad/include)

target_compile_options(Gryphon PRIVATE
				$<$<CONFIG:Debug>:-Wall;-Wextra;-g;-O0>
				$<$<CONFIG:RelWithDebugInfo>:-Wall;-Wextra;-g;-O0>
)

target_link_options(Gryphon PUBLIC $<$<CONFIG:Release>:-s>)

# Add our base layer
add_subdirectory(lib/base)

if(DEV)
	add_subdirectory(dev)
endif()




target_include_directories(Glad PUBLIC third_party/glad/include)

target_link_libraries(Gryphon
	PUBLIC
		Base
	PRIVATE
		Glad
		OpenGL::GL
)


if(UNIX AND NOT APPLE)
	target_link_libraries(Gryphon PRIVATE X11::X11)
endif()
