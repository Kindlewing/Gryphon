cmake_minimum_required(VERSION 3.24)
project(example C)
add_executable(example)
target_sources(example
	PRIVATE 
		main.c
)
target_link_libraries(example 
	PRIVATE
	Gryphon 
	OpenGL::GL
)
