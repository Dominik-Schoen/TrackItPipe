#!/bin/bash
# Build Qt

set -e

if [ $1 == "release" ]; then
  OPTIONS="-static"
else
  OPTIONS=""
fi

CORES=$(getconf _NPROCESSORS_ONLN)
JOBS=$(($CORES-1))

BASEDIR=$PWD/thirdparty/qt
DEPLOYDIR=$BASEDIR/install

QT_MAJOR=6
QT_MINOR=1
QT_BUGFIX=0

URL="https://download.qt.io/official_releases/qt/$QT_MAJOR.$QT_MINOR/$QT_MAJOR.$QT_MINOR.$QT_BUGFIX/single/qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX.tar.xz"

mkdir -p $BASEDIR

if [ -d $DEPLOYDIR ]; then
  rm -rf $DEPLOYDIR
fi
mkdir -p $DEPLOYDIR

pushd $BASEDIR

if [ ! -f qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX.tar.xz ]; then
  wget $URL
fi

if [ -d "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX" ]; then
  rm -rf "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX"
fi

tar -xf "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX.tar.xz"
pushd "qt-everywhere-src-$QT_MAJOR.$QT_MINOR.$QT_BUGFIX"

./configure $OPTIONS -ltcg -optimize-size -no-pch -prefix $DEPLOYDIR -release -opensource -confirm-license \
    -nomake examples -nomake tests -nomake tools \
    -skip qtscxml -skip qtwayland -skip qtdatavis3d -skip qtcharts \
    -skip qtquickcontrols2 -skip qtvirtualkeyboard -skip qtshadertools \
    -skip qttranslations -skip qtdoc -skip qt3d -skip qtnetworkauth \
    -skip qt5compat -skip qtcoap -skip qtlottie -skip qtmqtt \
    -skip qtopcua -skip qtquick3d -skip qtquicktimeline -skip qttools \
    -skip qtdeclarative -skip qtactiveqt

cmake --build . --parallel $JOBS
cmake --install .

popd
