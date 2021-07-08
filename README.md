# Bachelorthesis

Git project for my bachelor thesis

# TrackpointApp
## Development

Requirements:
- A recent C++ Compiler (only `gcc` is tested)
- The CMake build system
- Qt 6.1 or newer
- CPU power ;)

Windows instructions:
Use the MSYS2 environment and the mingw-w64 compiler environment`
Compiler and CMake: `pacman -S base-devel cmake`
Install qt6 with `pacman -S mingw-w64-x86_64-qt6`

To create a development build, clone the repo and go to the `trackpoint-app` folder.

Create a new folder `build`.

Then, go to the `build` folder and execute `cmake ..`. CMake will download all required dependencies automatically.

When CMake is done, run `make`. For better performance, run `make -jX` and replace X by the thread count of your machine minus 1.

You should now find the executable `TrackpointApp`.
