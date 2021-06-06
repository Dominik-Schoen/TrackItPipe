#!/bin/bash
# Builds all dependencies which are no clean CMake projects, so using FetchContent is not possible.

OPTIONS=""

for var in "$@"
do
  if [ $var == "release" ]; then
    OPTIONS+="release"
  fi
  if [ $var == "win-cross" ]; then
    OPTIONS+="win-cross"
  fi
done

thirdparty/qt.sh $OPTIONS

if [ $OPTIONS == "release" ]; then
  thirdparty/openscenegraph.sh release
  thirdparty/lib3mf.sh release
fi
