cmake_minimum_required(VERSION 3.20)
project(Gryphon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Gryphon STATIC)
add_library(Glad STATIC)

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

add_subdirectory(lib/base)
target_link_libraries(Gryphon Base Glad)
