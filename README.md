# Bachelorthesis

Git project for my bachelor thesis

# TrackpointApp
## Development

Note: Development on Windows is unsupported.

To create a development build, clone the repo and go to the `trackpoint-app` folder.

Create a new folder `build`.

Qt has to be build separately, we use a script for it. Build Qt by executing `./build-dependencies-first.sh`.

Then, go to the `build` folder and execute `cmake ..`. Cmake will download all other required dependencies automatically.

When cmake is done, run `make`. For better performance, run `make -jX` and replace X by the thread count of your machine minus 1.

You should now find the executable `TrackpointApp`.

## Release
WIP
