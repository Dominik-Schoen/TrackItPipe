#!/bin/bash
# Builds all dependencies which are no clean CMake projects, so using FetchContent is not possible.

if [ $1 == "release" ]; then
  OPTIONS="release"
else
  OPTIONS=""
fi

thirdparty/qt.sh $OPTIONS
thirdparty/openscenegraph.sh $OPTIONS
thirdparty/lib3mf.sh $OPTIONS
thirdparty/json.sh $OPTIONS
