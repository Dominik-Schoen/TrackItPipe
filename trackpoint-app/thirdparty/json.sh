#!/bin/bash
# Build Json

set -e

if [ $1 == "release" ]; then
  BASEDIR=$PWD/thirdparty/json-static
  OPTIONS="-DBUILD_SHARED_LIBS:BOOL=OFF"
else
  BASEDIR=$PWD/thirdparty/json
  OPTIONS=""
fi

DEPLOYDIR=$BASEDIR/install

JSON_TAG="v3.9.1"

JSON_REPO="https://github.com/nlohmann/json"

CONFIG="-DJSON_BuildTests:BOOL=OFF"

mkdir -p $BASEDIR

if [ -d $DEPLOYDIR ]; then
  rm -rf $DEPLOYDIR
fi
mkdir -p $DEPLOYDIR

pushd $BASEDIR

if [ -d "json" ]; then
  rm -rf "json"
fi

git clone --depth 1 --branch $JSON_TAG $JSON_REPO
pushd json

cmake . -DCMAKE_INSTALL_PREFIX=$DEPLOYDIR $CONFIG $OPTIONS
cmake --build . --parallel
cmake --install .

popd
