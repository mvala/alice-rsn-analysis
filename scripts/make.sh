#!/bin/bash

PROJECT_DIR="$(dirname $(dirname $(readlink -m $0)))"

[ "$1" = "clean" ] && { shift; rm -rf $PROJECT_DIR/build; }
[ -d $PROJECT_DIR/build ] ||  mkdir $PROJECT_DIR/build

cd $PROJECT_DIR/build || exit 1
#[ -f Makefile ] || cmake -DCMAKE_INSTALL_PREFIX=$PROJECT_DIR -DCMAKE_BUILD_TYPE=RelWithDebInfo ../
[ -f Makefile ] || cmake -DCMAKE_INSTALL_PREFIX=$PROJECT_DIR -DCMAKE_BUILD_TYPE=Debug ../
make -j$(nproc) $*

