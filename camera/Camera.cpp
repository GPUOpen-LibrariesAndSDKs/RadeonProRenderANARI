// Copyright 2020 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Camera.h"
// specific types
#include "Perspective.h"
#include "Orthographic.h"

namespace anari {
namespace rpr {

Camera::Camera(rpr_camera &camera) : m_camera(camera){}

Camera *Camera::createInstance(rpr_context &context, const char *type)
{
  rpr_camera camera;
  CHECK(rprContextCreateCamera(context, &camera))

  if(strcmp(type, "perspective") == 0){
      return new Perspective(camera);
  }
  if(strcmp(type, "orthographic") == 0){
    return new Orthographic(camera);
  }
  throw std::runtime_error("could not create camera");
}

void Camera::commit()
{
  vec3 pos = getParam<vec3>("position", vec3(0.f));
  vec3 at = getParam<vec3>("direction", vec3(0.f, 0.f, -1.f)) + pos;
  vec3 up = normalize(getParam<vec3>("up", vec3(0.f, 1.f, 0.f)));
  mat4 transform = getParam<mat4x3>("transform", mat4x3(1));
  CHECK(rprCameraLookAt(m_camera, pos[0], pos[1], pos[2], at[0], at[1], at[2], up[0], up[1], up[2]))

  if(transform != mat4(1)){ // transform overrides camera pos, direction and up
    CHECK(rprCameraSetTransform(m_camera, false, value_ptr(transform)))
  }
}

void Camera::attachToScene(rpr_scene scene){
    CHECK(rprSceneSetCamera(scene, m_camera))
}

Camera::~Camera(){
  CHECK(rprObjectDelete(m_camera))
}

} // namespace rpr

ANARI_TYPEFOR_DEFINITION(rpr::Camera *);

} // namespace anari
