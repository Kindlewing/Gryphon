#!/usr/bin/env bash
set echo on

application="engine"
generate_compile_commands=false

cflags="-g -Wall -Wextra -O0"
link_flags="-lgl -lX11"

obj_dir="obj"
mkdir $obj_dir
start=$(date +%s%3N)
clang -c src/gryphon_unity.c $cflags $link_flags -o $obj_dir/gryphon.o
ar rcs "lib${application}.a" $obj_dir/gryphon.o
end=$(date +%s%3N)
echo "$application build finished in $((end - start))ms"
