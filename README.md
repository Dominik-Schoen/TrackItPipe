# TrackItPipe
## Development

Requirements:
- A recent C++ Compiler (only `gcc` is tested)
- The CMake build system
- Qt 6.1 or newer
- CPU power ;)

Windows instructions:
- Use the MSYS2 environment and the mingw-w64 compiler environment. A great tutorial: https://github.com/orlp/dev-on-windows/wiki/Installing-GCC--&-MSYS2
- Compiler and CMake: `pacman -S base-devel cmake git`
- Install qt6 with `pacman -S mingw-w64-x86_64-qt6`

To create a development build, clone the repo and go to the `trackpoint-app` folder.

Create a new folder `build`.

Then, go to the `build` folder and execute `cmake ..` (or `cmake -G "Unix Makefiles" ..` if you are on windows). CMake will download all required dependencies automatically.

When CMake is done, run `make`. For better performance, run `make -jX` and replace X by the thread count of your machine minus 1.

You should now find the executable `TrackpointApp`.

## How to cite TrackItPipe

Below are the BibTex entries to cite TrackItPipe

```bibtex
@inproceedings{schoen2022tracktipipe,
title = {TrackItPipe: A Fabrication Pipeline To Incorporate Location and Rotation Tracking Into 3D Printed Objects},
author = {Schön, Dominik and Kosch, Thomas and Schmitz, Martin and Müller, Florian and Günther, Sebastian and Kreutz, Johannes and Mühlhäuser, Max},
booktitle = {The 35rd Annual ACM Symposium on User Interface Software and Technology Adjunct Proceedings},
year = {2022},
doi = {10.1145/3526114.3558719},
organization = {ACM}
}
```