#!/bin/bash
# Build Qt

set -e

OPTIONS=""
CMAKE_OPTIONS=""

for var in "$@"
do
  if [ $var == "release" ]; then
    OPTIONS+="-static "
  fi
  if [ $var == "win-cross" ]; then
    CMAKE_OPTIONS+="-DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw-w64.cmake"
  fi
done

CORES=$(getconf _NPROCESSORS_ONLN)
JOBS=$(($CORES-1))

BASEDIR=$PWD/thirdparty/qt
DEPLOYDIR=$BASEDIR/install

QT_MAJOR=6
QT_MINOR=1
QT_BUGFIX=0

UNAME_OUT="$(uname -s)"
case "${UNAME_OUT}" in
    Linux*)     MACHINE=Linux;;
    Darwin*)    MACHINE=Mac;;
    CYGWIN*)    MACHINE=Cygwin;;
    MINGW*)     MACHINE=MinGw;;
    *)          MACHINE="UNKNOWN:${UNAME_OUT}"
esac

if [ $MACHINE == "Linux" ]; then
  OPTIONS+="-xcb "
fi

URL="https://download.qt.io/official_releases/qt/$QT_MAJOR.$QT_MINOR/$QT_MAJOR.$QT_MINOR.$QT_BUGFIX/single/qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX.tar.xz"

mkdir -p "$BASEDIR"

if [ -d "$DEPLOYDIR" ]; then
  rm -rf "$DEPLOYDIR"
fi
mkdir -p "$DEPLOYDIR"

pushd "$BASEDIR"

if [ ! -f qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX.tar.xz ]; then
  wget $URL
fi

if [ -d "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX" ]; then
  rm -rf "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX"
fi

tar -xf "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX.tar.xz"
pushd "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX"

./configure $OPTIONS -ltcg -optimize-size -no-pch -prefix "$DEPLOYDIR" -release -opensource -confirm-license \
    -nomake examples -nomake tests -nomake tools \
    -skip qtscxml -skip qtdatavis3d -skip qtcharts \
    -skip qtquickcontrols2 -skip qtvirtualkeyboard -skip qtshadertools \
    -skip qttranslations -skip qtdoc -skip qt3d -skip qtnetworkauth \
    -skip qt5compat -skip qtcoap -skip qtlottie -skip qtmqtt \
    -skip qtopcua -skip qtquick3d -skip qtquicktimeline -skip qttools \
    -skip qtactiveqt

cmake --build . --parallel $JOBS $CMAKE_OPTIONS
cmake --install .

popd
