cmake_minimum_required(VERSION 3.24)
project(dev)

add_executable(dev)
target_sources(dev
	PRIVATE
	src/main.c
)
target_link_libraries(dev PRIVATE Gryphon Glad)
