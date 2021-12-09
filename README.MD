# RadeonProRender ANARI device
## Description
This is implementation of ANARI device based on RadeonProRender engine
## Dependencies
[ANARI SDK](https://github.com/KhronosGroup/ANARI-SDK) must be installed on computer before RPR device building.
## Building the device
The repository uses CMake 3.11+ to build the device
For example, to build (must be in a separate directory from the source directory), you can do:
#### UNIX-like systems
```shell
cd path/to/source
mkdir build
cd build
cmake ..
cmake --build .
```
#### Windows
Visual studio 17 or 19 must be installed. Building using MinGW may produce an error!
* create build directory into directory with source code
* run `cmake ..` from build directory
* run `cmake --build .` from build directory

Also, you can build project from Visual Studio IDE using CMake extension.
