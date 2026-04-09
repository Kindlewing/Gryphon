#!/usr/bin/env bash
config=${1:-debug}
application="engine"

cflags="-g -Iinclude -Wall -Wextra -O0"

config=${1:-debug}

if [ "$config" = "release" ]; then
    cflags="-Iinclude -Wall -Wextra -O2"
else
    cflags="-Iinclude -Wall -Wextra -g -O0"
fi

build_dir="build"
obj_dir="$build_dir/obj"

mkdir -p $build_dir
mkdir -p $obj_dir

clang -c src/gryphon_unity.c $cflags -o $obj_dir/gryphon.o
ar rcs "$build_dir/lib${application}.a" $obj_dir/gryphon.o
