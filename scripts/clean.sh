#!/bin/bash

PROJECT_DIR="$(dirname $(dirname $(readlink -m $0)))"

for d in bin lib lib64 share pars; do
  [ -d $PROJECT_DIR/$d ] && rm -rf $PROJECT_DIR/$d
done
