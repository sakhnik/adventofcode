#!/bin/bash -ex

cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p BUILD

(
cd BUILD
CC=clang CXX=clang++ cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=Yes \
    ..
)

ln -sf BUILD/compile_commands.json .
