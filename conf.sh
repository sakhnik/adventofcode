#!/bin/bash -ex

cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p BUILD

(
cd BUILD
# Full path to clang to avoid rebuilds because of incorrect system header paths
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=Yes \
    ..
)

ln -sf BUILD/compile_commands.json .
