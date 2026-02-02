cmake_minimum_required(VERSION 3.20)
project(Base C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Base STATIC)
target_sources(Base
	PRIVATE
			src/base_math.c
			src/profile.c
			src/string8.c
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			include
		FILES
			include/arena.h
			include/base_math.h
			include/macros.h
			include/profile.h
			include/string8.h
			include/typedefs.h
)

if(WIN32)
	target_sources(Base PRIVATE src/arena_win32.c)
elseif(UNIX AND NOT APPLE)
	target_sources(Base PRIVATE src/arena_linux.c)
endif()
