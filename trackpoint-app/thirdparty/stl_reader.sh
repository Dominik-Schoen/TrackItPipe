#!/bin/bash
# Build stl_reader

set -e

BASEDIR=$PWD/thirdparty/stl_reader
DEPLOYDIR=$BASEDIR/install

STL_READER_REPO="https://github.com/sreiter/stl_reader"

mkdir -p "$BASEDIR"

if [ -d "$DEPLOYDIR" ]; then
  rm -rf "$DEPLOYDIR"
fi
mkdir -p "$DEPLOYDIR"

pushd "$BASEDIR"

if [ -d "stl_reader" ]; then
  rm -rf "stl_reader"
fi

git clone --depth 1 $STL_READER_REPO

cp stl_reader/stl_reader.h "$DEPLOYDIR"

popd
