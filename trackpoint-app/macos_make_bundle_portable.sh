#!/bin/bash

set -e

cp build/_deps/openscenegraph-build/lib/libosgViewer.3.6.5.dylib build/TrackpointApp.app/Contents/MacOS/libosgViewer.dylib
cp build/_deps/openscenegraph-build/lib/libosgGA.3.6.5.dylib build/TrackpointApp.app/Contents/MacOS/libosgGA.dylib
cp build/_deps/openscenegraph-build/lib/libosgText.3.6.5.dylib build/TrackpointApp.app/Contents/MacOS/libosgText.dylib
cp build/_deps/openscenegraph-build/lib/libosgDB.3.6.5.dylib build/TrackpointApp.app/Contents/MacOS/libosgDB.dylib
cp build/_deps/openscenegraph-build/lib/libosgUtil.3.6.5.dylib build/TrackpointApp.app/Contents/MacOS/libosgUtil.dylib
cp build/_deps/openscenegraph-build/lib/libosg.3.6.5.dylib build/TrackpointApp.app/Contents/MacOS/libosg.dylib
cp build/_deps/openscenegraph-build/lib/libOpenThreads.3.3.1.dylib build/TrackpointApp.app/Contents/MacOS/libOpenThreads.dylib
cp build/_deps/lib3mf-build/lib3mf.2.2.0.0.dylib build/TrackpointApp.app/Contents/MacOS/lib3mf.2.dylib

install_name_tool -add_rpath "@executable_path/." build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libosgViewer.161.dylib "@loader_path/libosgViewer.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libosgGA.161.dylib "@loader_path/libosgGA.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libosgText.161.dylib "@loader_path/libosgText.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libosgDB.161.dylib "@loader_path/libosgDB.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libosgUtil.161.dylib "@loader_path/libosgUtil.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libosg.161.dylib "@loader_path/libosg.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
install_name_tool -change /Volumes/Daten/bachelorthesis/trackpoint-app/build/_deps/openscenegraph-build/lib/libOpenThreads.21.dylib "@loader_path/libOpenThreads.dylib" build/TrackpointApp.app/Contents/MacOS/TrackpointApp
