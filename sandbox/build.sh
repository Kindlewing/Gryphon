#!/usr/bin/env bash
config=${1:-debug}
application="sandbox"

if [ "$config" = "release" ]; then cflags="-Wall -Wextra -O2"
else
    cflags="-Wall -Wextra -g -O0"
fi

link_flags="-lGL -lX11"
build_dir="build"

mkdir -p $build_dir

clang src/main.c $cflags $link_flags -o $build_dir/sandbox
