cmake_minimum_required(VERSION 3.24)
project(Gryphon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_EXAMPLE "Build the example project" OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Gryphon STATIC)
add_library(Glad STATIC)

find_package(OpenGL REQUIRED)

# Gryphon sources
target_sources(Gryphon
	PRIVATE
		src/gryphon/gryphon.c
		src/gryphon/platform/x11.c
		src/gryphon/render/shader.c
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			include
		FILES
			include/gryphon.h
			include/platform/platform.h
			include/platform/x11.h
			include/render/shader.h
)
# Glad sources
target_sources(Glad
	PRIVATE
		third_party/glad/src/glad.c
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			third_party/glad/include
		FILES
			third_party/glad/include/glad.h
			third_party/glad/include/KHR/khrplatform.h
)
# Add our base layer
add_subdirectory(lib/base)

if(BUILD_EXAMPLE)
	add_subdirectory(examples)
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(Gryphon
        PRIVATE
			X11::X11
    )
endif()

# Link everything
target_link_libraries(Gryphon 
	PUBLIC 
	Base 
	PRIVATE 
	Glad
	OpenGL::GL
)
