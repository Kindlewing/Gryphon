cmake_minimum_required(VERSION 3.20)
project(Gryphon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Gryphon)
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
add_subdirectory(lib/base)
target_link_libraries(Gryphon Base)
