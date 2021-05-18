#!/bin/bash
# Build OpenSceneGraph

set -e

if [ $1 == "release" ]; then
  OPTIONS="-DDYNAMIC_OPENSCENEGRAPH:BOOL=OFF -DDYNAMIC_OPENTHREADS:BOOL=OFF -DOSG_GL_LIBRARY_STATIC:BOOL=ON"
else
  OPTIONS=""
fi

CORES=$(getconf _NPROCESSORS_ONLN)
JOBS=$(($CORES-1))

BASEDIR=$PWD/thirdparty/openscenegraph
DEPLOYDIR=$BASEDIR/install

OPENSCENEGRAPH_TAG="OpenSceneGraph-3.6.5"

OPENSCENEGRAPH_REPO="https://github.com/openscenegraph/OpenSceneGraph"

CONFIG="-DBUILD_OSG_EXAMPLES:BOOL=OFF -DBUILD_OSG_APPLICATIONS:BOOL=OFF -DOSG_TEXT_USE_FONTCONFIG:BOOL=OFF"

mkdir -p $BASEDIR

if [ -d $DEPLOYDIR ]; then
  rm -rf $DEPLOYDIR
fi
mkdir -p $DEPLOYDIR

pushd $BASEDIR

if [ -d "OpenSceneGraph" ]; then
  rm -rf "OpenSceneGraph"
fi

git clone --depth 1 --branch $OPENSCENEGRAPH_TAG $OPENSCENEGRAPH_REPO
pushd OpenSceneGraph

cmake . -DCMAKE_INSTALL_PREFIX=$DEPLOYDIR $CONFIG $OPTIONS
cmake --build . --parallel $JOBS
cmake --install .

popd
