#!/bin/bash -ex

this_dir="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"
cd "$this_dir"
mkdir -p BUILD

(
cd BUILD
# Full path to clang to avoid rebuilds because of incorrect system header paths
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=Yes \
    "$this_dir"
)

ln -sf BUILD/compile_commands.json .
