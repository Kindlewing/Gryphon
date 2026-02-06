cmake_minimum_required(VERSION 3.24)
project(dev)

add_executable(dev)
target_sources(dev
	PRIVATE
	src/main.c
)

target_compile_options(
	dev
	PRIVATE
	$<$<CONFIG:Debug>:-Wall -Wextra -g -O0>
)
target_link_libraries(dev PRIVATE Gryphon Glad)
