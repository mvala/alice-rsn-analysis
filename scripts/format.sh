#!/bin/bash

hash clang-format 2>/dev/null || { echo >&2 "I require clang-format , but it's not installed.  Aborting."; exit 1; }

PROJECT_DIR="$(dirname $(dirname $(readlink -nf $0)))"
find $PROJECT_DIR -type f \( -name "*.cxx" -o -name "*.h" \) -not -path "*build*" | xargs clang-format -i -style=llvm
find $PROJECT_DIR/macros/analysis -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" \) | xargs clang-format -i -style=llvm

