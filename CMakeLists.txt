cmake_minimum_required(VERSION 3.11)

## Language setup

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Establish project

project(anari_library_rpr LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
include(GNUInstallDirs)
include(FetchContent)

function(fetch_project)
    cmake_parse_arguments(FETCH_SOURCE "" "NAME;URL;DESTINATION" "" ${ARGN})

    FetchContent_Populate(${FETCH_SOURCE_NAME}
            URL ${FETCH_SOURCE_URL}
            SOURCE_DIR ${CMAKE_BINARY_DIR}/${FETCH_SOURCE_DESTINATION}
            )

    set(
            "${FETCH_SOURCE_NAME}_LOCATION"
            ${CMAKE_BINARY_DIR}/${FETCH_SOURCE_DESTINATION}
            PARENT_SCOPE
    )
endfunction()

## ANARI ##

find_package(anari REQUIRED)

## glm ##

fetch_project(
        NAME glm_src
        URL https://github.com/g-truc/glm/archive/0.9.9.8.zip
        DESTINATION glm
)

set(glm_DIR "${glm_src_LOCATION}/cmake/glm")
find_package(glm REQUIRED)

add_library(glm INTERFACE IMPORTED GLOBAL)
target_link_libraries(glm INTERFACE glm::glm)

## RPR ##
fetch_project(
  NAME rpr_sdk
	URL https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRenderSDK/archive/refs/tags/v2.2.12.zip
  DESTINATION rpr
)

set(RPR_DIR "${rpr_sdk_LOCATION}/RadeonProRender")

if(WIN32)
    set(RPR_LIB_DIR "${RPR_DIR}/binWin64")
    set(CMAKE_FIND_LIBRARY_SUFFIXES .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})
else()
    if(APPLE)
        set(RPR_LIB_DIR "${RPR_DIR}/binMacOS")
    else()
        # there is also a Centos version
        set(RPR_LIB_DIR "${RPR_DIR}/binUbuntu18")
    endif()
endif()

# find RPR plugin renderers to install

find_library(RPR_LIB
	NAMES RadeonProRender64 libRadeonProRender64
    HINTS "${RPR_LIB_DIR}")
find_library(RPR_PLUGIN_NORTHSTAR
	NAMES Northstar64 libNorthstar64
    HINTS "${RPR_LIB_DIR}")
find_library(RPR_PLUGIN_HYBRID_PRO
	NAMES HybridPro HybridPro${CMAKE_SHARED_LIBRARY_SUFFIX}
    HINTS "${RPR_LIB_DIR}")

add_library(${PROJECT_NAME} SHARED
  Object.cpp
  RPRDevice.cpp
  Array.cpp

  camera/Camera.cpp
  camera/Perspective.cpp
  camera/Orthographic.cpp

  frame/Frame.cpp

  material/Material.cpp
  material/MatteMaterial.cpp

  sampler/Sampler.cpp
  sampler/TransformSampler.cpp
  sampler/PrimitiveSampler.cpp
  sampler/image/Image.cpp
  sampler/image/Image1D.cpp
  sampler/image/Image2D.cpp
  sampler/image/Image3D.cpp
  sampler/util/TransformNode.cpp

  renderer/Renderer.cpp
  renderer/PathTracerRenderer.cpp

  scene/Instance.cpp
  scene/Group.cpp
  scene/World.cpp

  scene/geometry/Surface.cpp
  scene/geometry/Geometry.cpp
  scene/geometry/mesh/Mesh.cpp
  scene/geometry/primitives/Primitives.cpp
  scene/geometry/primitives/Spheres.cpp
  scene/geometry/primitives/Cylinders.cpp

  scene/geometry/attributes/Attribute.cpp
  scene/geometry/attributes/PerShapeAttribute.cpp
  scene/geometry/attributes/PrimVarAttribute.cpp

  scene/lights/Light.cpp
  scene/lights/analytic/AnalyticLight.cpp
  scene/lights/analytic/DirectionalLight.cpp
  scene/lights/analytic/PointLight.cpp
  scene/lights/analytic/SpotLight.cpp
  scene/lights/analytic/HdriLight.cpp
  scene/lights/analytic/RingLight.cpp
  scene/lights/area/QuadLight.cpp

  scene/volume/SpatialField.cpp
  scene/volume/StucturedRegularSpatialField.cpp
  scene/volume/Volume.cpp
  scene/volume/ScivisVolume.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE -Drpr_device_EXPORTS)
target_include_directories(${PROJECT_NAME} PRIVATE ${RPR_DIR}/inc)
if(WIN32)
    target_link_directories(${PROJECT_NAME} PRIVATE "${RPR_DIR}/libWin64")
    target_link_libraries(${PROJECT_NAME} PRIVATE anari::anari anari::anari_utilities glm RadeonProRender64)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE anari::anari anari::anari_utilities glm ${RPR_LIB})
endif()


# Installation

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if (WIN32)
	if(NOT ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
		set(DESTINATION_DIR $(OutDir))
	else()
		set(DESTINATION_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	endif()
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${RPR_LIB_DIR}" "${DESTINATION_DIR}"
		COMMENT "Copy RPR SDK libraries")
else()
  # Copy RPR libs to build directory
  file(COPY ${RPR_LIB}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  file(COPY ${RPR_PLUGIN_NORTHSTAR}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  if (NOT APPLE)
    file(COPY ${RPR_PLUGIN_HYBRID_PRO}
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  endif()

  # Install RPR libs in final destination
  if(NOT APPLE)
      install(
        FILES
          ${RPR_LIB}
          ${RPR_PLUGIN_NORTHSTAR}
          ${RPR_PLUGIN_HYBRID_PRO}
        DESTINATION
          ${CMAKE_INSTALL_LIBDIR}
      )
  else()
      install(
        FILES
          ${RPR_LIB}
          ${RPR_PLUGIN_NORTHSTAR}
        DESTINATION
          ${CMAKE_INSTALL_LIBDIR}
      )
  endif()
endif()
