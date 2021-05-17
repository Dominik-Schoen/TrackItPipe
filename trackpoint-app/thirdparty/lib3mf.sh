#!/bin/bash
# Build lib3mf

set -e

if [ $1 == "release" ]; then
  BASEDIR=$PWD/thirdparty/lib3mf-static
  OPTIONS="-DBUILD_SHARED_LIBS:BOOL=OFF"
else
  BASEDIR=$PWD/thirdparty/lib3mf
  OPTIONS=""
fi

DEPLOYDIR=$BASEDIR/install

LIB3MF_TAG="v2.1.1"

LIB3MF_REPO="https://github.com/3MFConsortium/lib3mf"

CONFIG="-DLIB3MF_TESTS:BOOL=OFF"

mkdir -p $BASEDIR

if [ -d $DEPLOYDIR ]; then
  rm -rf $DEPLOYDIR
fi
mkdir -p $DEPLOYDIR

pushd $BASEDIR

if [ -d "lib3mf" ]; then
  rm -rf "lib3mf"
fi

git clone --depth 1 --branch $LIB3MF_TAG $LIB3MF_REPO
pushd lib3mf

cmake . -DCMAKE_INSTALL_PREFIX=$DEPLOYDIR $CONFIG $OPTIONS
cmake --build . --parallel
cmake --install .

popd
