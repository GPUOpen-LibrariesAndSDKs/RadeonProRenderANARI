#include "Spheres.h"
#include "Math/mathutils.h"

namespace anari::rpr {

Spheres::Spheres(rpr_context &context) : Geometry(context) {}

void Spheres::commit(){

  if (!hasParam("vertex.position"))
    throw std::runtime_error("missing 'vertex.position' on sphere geometry");

  auto vertexData = getParamObject<Array1D>("vertex.position");
  auto radiusData = getParamObject<Array1D>("vertex.radius");
  auto globalRadius = getParam<float>("radius", 1.f);

  if (radiusData && radiusData->size()!=vertexData->size()){
    throw std::runtime_error("'vertex.position' and 'vertex.radius' sizes are incompatible");
  }

  for(rpr_shape shape : m_shapes){
    CHECK(rprObjectDelete(shape))
  }

  m_shapes.clear();
  m_shapes.resize(vertexData->size()+1);

  if(!m_base_sphere){
    createBaseSphere();
  }

  resetBounds();

  for(int vertexNumber=0; vertexNumber < vertexData->size(); vertexNumber++){
    vec3 vertex = vertexData->dataAs<vec3>()[vertexNumber];
    float radius = radiusData ? radiusData->dataAs<float>()[vertexNumber] : globalRadius;
    rprContextCreateInstance(m_context, m_base_sphere, &(m_shapes[vertexNumber]));

    //transform
    RadeonProRender::matrix m = RadeonProRender::matrix(radius,0,0,vertex.x,0,radius,0,vertex.y,0,0,radius,vertex.z,0,0,0,1);

    CHECK(rprShapeSetTransform(m_shapes[vertexNumber], RPR_TRUE, &m.m00))
    CHECK(rprShapeSetObjectID(m_shapes[vertexNumber], vertexNumber))

    //bounds
    m_bounds.upper.x = max(m_bounds.upper.x, vertex.x + radius);
    m_bounds.upper.y = max(m_bounds.upper.y, vertex.y + radius);
    m_bounds.upper.z = max(m_bounds.upper.z, vertex.z + radius);
    m_bounds.lower.x = min(m_bounds.lower.x, vertex.x - radius);
    m_bounds.lower.y = min(m_bounds.lower.y, vertex.y - radius);
    m_bounds.lower.z = min(m_bounds.lower.z, vertex.z - radius);
  }
  m_shapes[vertexData->size()] = m_base_sphere; //attach invisible base sphere last
  markUpdated();
}

void Spheres::createBaseSphere(){  //creates base sphere with center 0,0,0 and radius 1
  float const R = 1.f/(float)(rings -1);
  float const S = 1.f/(float)(sectors -1);

  std::vector<rpr_float> vertices;
  std::vector<rpr_int> indices;

  int num_faces = (rings-1) * (sectors-1);

  vertices.resize(rings * sectors * 3);
  indices.resize(num_faces * 4);

  auto v = vertices.begin();
  auto i = indices.begin();

  for(int r=0; r< rings; r++) for(int s=0; s< sectors; s++){
      float const y = sin((M_PI / 2.f) + M_PI * R * r);
      float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
      float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

      *v++ = x;
      *v++ = y;
      *v++ = z;

  }

  for(int r=0; r<rings-1; r++) for(int s=0; s< sectors-1; s++){

      *i++ = r * sectors + s;
      *i++ = r * sectors + (s+1);
      *i++ = (r+1) * sectors + (s+1);
      *i++ = (r+1) * sectors + s;
  }

  std::vector<rpr_int> faces(num_faces, 4);

  CHECK(rprContextCreateMesh(m_context, (rpr_float *) vertices.data(), vertices.size() / 3, sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, (rpr_int * ) indices.data(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &m_base_sphere))
  CHECK(rprShapeSetVisibility(m_base_sphere, false)) //this is invisible 'original' sphere. It's instances will be visible
}

Spheres::~Spheres(){
  for(rpr_shape shape : m_shapes){
    CHECK(rprObjectDelete(shape))
  }
  if(m_base_sphere){
    CHECK(rprObjectDelete(m_base_sphere))
  }
}

}//anari