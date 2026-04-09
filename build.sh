#!/usr/bin/env bash

generate_compile_commands=false
build_engine=true
build_sandbox=true
config="debug"

usage() {
    echo "Usage: $0 [-c] [-e] [-s] [-r]"
    echo "  -c    Generate compile commands"
    echo "  -e    Build engine only"
    echo "  -s    Build sandbox only"
    echo "  -r    Release configuration"
    exit 1
}

while getopts ":cesr" opt; do
    case $opt in
        c) generate_compile_commands=true ;;
        e) build_sandbox=false ;;
        s) build_engine=false ;;
        r) config="release" ;;
        *) usage ;;
    esac
done

start=$(date +%s%3N)

if $build_engine; then
    pushd engine > /dev/null || exit
    if $generate_compile_commands; then
        bear -- ./build.sh "$config"
    else
        ./build.sh "$config"
    fi
    popd > /dev/null || exit
fi

if $build_sandbox; then
    pushd sandbox > /dev/null || exit
    if $generate_compile_commands; then
        bear -- ./build.sh "$config"
    else
        ./build.sh "$config"
    fi
    popd > /dev/null || exit
fi

end=$(date +%s%3N)
echo "Build finished in $((end - start))ms"
