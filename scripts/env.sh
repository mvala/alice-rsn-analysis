#!/bin/bash

PROJECT_DIR="$(dirname $(dirname $(readlink -m ${BASH_ARGV[0]})))"

export PATH="$PROJECT_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$PROJECT_DIR/lib:$PROJECT_DIR/lib64:$LD_LIBRARY_PATH"
export ER_MACRO_DIR="$PROJECT_DIR/macros"
