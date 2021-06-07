#!/bin/bash
# Build Qt

set -e

OPTIONS=""
WINCROSS="false"

for var in "$@"
do
  if [ $var == "release" ]; then
    OPTIONS+="-DBUILD_SHARED_LIBS=OFF "
  fi
  if [ $var == "win-cross" ]; then
    WINCROSS="true"
    OPTIONS+="-DCMAKE_TOOLCHAIN_FILE=../../../toolchain-mingw-w64.cmake -DBUILD_qtwayland=OFF "
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

if [[ ($MACHINE == "Linux") && ($WINCROSS == "false") ]]; then
  OPTIONS+="-DINPUT_xcb=yes "
fi

if [ $MACHINE == "Mac" ]; then
  OPTIONS+="-DBUILD_qtwayland=OFF "
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

cmake $OPTIONS -DBUILD_qtscxml=OFF -DBUILD_qtdatavis3d=OFF -DBUILD_qtcharts=OFF\
  -DBUILD_qtquickcontrols2=OFF -DBUILD_qtvirtualkeyboard=OFF -DBUILD_qtshadertools=OFF\
  -DBUILD_qttranslations=OFF -DBUILD_qtdoc=OFF -DBUILD_qt3d=OFF -DBUILD_qtnetworkauth=OFF\
  -DBUILD_qt5compat=OFF -DBUILD_qtcoap=OFF -DBUILD_qtlottie=OFF -DBUILD_qtmqtt=OFF\
  -DBUILD_qtopcua=OFF -DBUILD_qtquick3d=OFF -DBUILD_qtquicktimeline=OFF -DBUILD_qttools=OFF\
  -DBUILD_qtactiveqt=OFF -DBUILD_WITH_PCH=OFF\
  -DCMAKE_INSTALL_PREFIX="$DEPLOYDIR"\
  -DQT_BUILD_EXAMPLES=FALSE -DQT_BUILD_TESTS=FALSE\
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE=ON -DINPUT_optimize_size=yes -G Ninja

cmake --build . --parallel $JOBS
cmake --install .

popd
