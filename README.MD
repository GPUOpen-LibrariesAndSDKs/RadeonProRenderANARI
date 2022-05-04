# RadeonProRender ANARI device
## Description
This is implementation of ANARI device based on RadeonProRender engine
## Implementation details
### Camera
* supported types: perspective and orthographic
* `transform` parameter overrides `position`, `direction` and `up` parameters
* `imageRegion` parameter is not supported now
* `focusDistance` and `apertureRadius` parameters are only supported by perspective camera
* both cameras have an additional parameter `sensorHeight`. Sensor width will be calculated using `aspect` parameter. Default sensor height is 24 mm
* orthographic camera has an additional parameter `orthoHeight`. It determines the area which the Orthographic camera projection will cover. Width will be calculated using `aspect` parameter. Default value is 1
### Geometry
* curve geometry type is not supported now
* general parameters (`primitive.color`, `primitive.attribute`, `primitive.id`) are not supported
* `vertex.color` can only accept float32 types (`FLOAT32`, `FLOAT32_VEC2`, `FLOAT32_VEC3`, `FLOAT32_VEC4`)
* `vertex.normal` can only accept `FLOAT32` values
* `primitive.index` can only accept uint32 values and vectors
### Volumes
* data in spatial fields can be only in float32 format (`ARRAY3D` of `FLOAT32`)
* `color` can only accept float32 types (`FLOAT32`, `FLOAT32_VEC2`, `FLOAT32_VEC3`, `FLOAT32_VEC4`)
* `color.position` and `opacity.position` parameters are not supported
### Features which are not implemented yet
* object introspection
* stereo mode of cameras
* depth framebuffer
* curve geometry
* KHR_AUXILIARY_BUFFERS
* ANARI_KHR_TRANSFORMATION_MOTION_BLUR
## Dependencies
[ANARI SDK](https://github.com/KhronosGroup/ANARI-SDK) must be installed on computer before RPR device building.
## Building the device
The repository uses CMake 3.11+ to build the device
For example, to build (must be in a separate directory from the source directory), you can do:
### UNIX-like systems
```shell
cd path/to/source
mkdir build
cd build
cmake ..
cmake --build . -t install
```
It's important to have directory with installed device in LD_LIBRARY_PATH
### Windows
Visual studio 17 or 19 must be installed. Building using MinGW may produce an error!
* create build directory into directory with source code
* run `cmake ..` from build directory
* run `cmake --build .` from build directory

Also, you can build project from Visual Studio IDE using CMake extension.
