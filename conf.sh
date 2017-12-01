#!/bin/bash -ex

this_dir=`readlink -f $(dirname ${BASH_SOURCE[0]})`

cd `dirname ${BASH_SOURCE[0]}`
mkdir -p BUILD

mkdir -p /tmp/aoc2017-build
bindfs -n /tmp/aoc2017-build BUILD

(
cd BUILD
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Debug ..
)
