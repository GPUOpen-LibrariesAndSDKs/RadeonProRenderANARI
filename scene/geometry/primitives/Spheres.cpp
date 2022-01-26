#include "Spheres.h"
#include "Math/mathutils.h"

namespace anari::rpr {

Spheres::Spheres(rpr_context context, rpr_material_system materialSystem) : Primitives(context, materialSystem){
  createBaseShape();
}

void Spheres::commit(){

  auto vertexData = getParamObject<Array1D>("vertex.position");
  auto radiusData = getParamObject<Array1D>("vertex.radius");
  auto indexData = getParamObject<Array1D>("primitive.index");
  auto globalRadius = getParam<float>("radius", 1.f);

  if (!vertexData)
    throw std::runtime_error("missing 'vertex.position' on sphere geometry");

  if (radiusData && radiusData->size()!=vertexData->size()){
    throw std::runtime_error("'vertex.position' and 'vertex.radius' sizes are incompatible");
  }

  m_positions.clear();
  m_indices.clear();
  m_radius.clear();

  if(indexData){
    m_num_primitives = indexData->size();
    for(int indexNumber = 0; indexNumber < m_num_primitives; indexNumber++)
    {
      m_indices.push_back(indexData->dataAs<int>()[indexNumber]);
    }
  }
  else // index data is not provided, we should use [0,1,2,3,...]
  {
    m_num_primitives = vertexData->size();
    for(int indexNumber = 0; indexNumber < m_num_primitives; indexNumber++)
    {
      m_indices.push_back(indexNumber);
    }
  }

  resetBounds();

  for(int vertexNumber=0; vertexNumber < vertexData->size(); vertexNumber++){
    vec3 vertex = vertexData->dataAs<vec3>()[vertexNumber];
    float radius = radiusData ? radiusData->dataAs<float>()[vertexNumber] : globalRadius;

    m_positions.push_back(vertex);
    m_radius.push_back(radius);

    //bounds
    m_bounds.upper.x = max(m_bounds.upper.x, vertex.x + radius);
    m_bounds.upper.y = max(m_bounds.upper.y, vertex.y + radius);
    m_bounds.upper.z = max(m_bounds.upper.z, vertex.z + radius);
    m_bounds.lower.x = min(m_bounds.lower.x, vertex.x - radius);
    m_bounds.lower.y = min(m_bounds.lower.y, vertex.y - radius);
    m_bounds.lower.z = min(m_bounds.lower.z, vertex.z - radius);
  }
}

mat4 Spheres::generatePrimitiveTransform(int primitive_number)
{
  vec3 vertex = m_positions[m_indices[primitive_number]];
  float radius = m_radius[m_indices[primitive_number]];
  return transpose(mat4(radius,0,0,vertex.x,0,radius,0,vertex.y,0,0,radius,vertex.z,0,0,0,1));
}

void Spheres::createBaseShape(){  //creates base sphere with center 0,0,0 and radius 1
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

  CHECK(rprContextCreateMesh(m_context, (rpr_float *) vertices.data(), vertices.size() / 3, sizeof(rpr_float) * 3, nullptr, 0, 0, nullptr, 0, 0, (rpr_int * ) indices.data(), sizeof(rpr_int), nullptr, 0, nullptr, 0, faces.data(), num_faces, &m_base_shape))
  CHECK(rprShapeSetVisibility(m_base_shape, false)) //this is invisible 'original' sphere. It's instances will be visible
}

void Spheres::generateDefaultIndex() {

}

}//anari