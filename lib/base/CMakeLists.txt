cmake_minimum_required(VERSION 3.20)\
project(Base C)

add_library(Base)
target_sources(Base
	PRIVATE 
		 src/arena_linux.c
		 src/arena_win32.c
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
