// Include own headers
#include "STLImport.hpp"

// Include dependencies
#include "stl_reader.h"

void STLImport::readSTL(std::string file, std::vector<Lib3MF::sPosition>* verticesBuffer, std::vector<Lib3MF::sTriangle>* triangleBuffer) {
  stl_reader::StlMesh<float, unsigned int> mesh(file);
  unsigned int vertexId = 0;
  for (size_t i = 0; i < mesh.num_tris(); i++) {
    Lib3MF_uint32 indices[3] = {0, 0, 0};
    for (size_t corner = 0; corner < 3; corner++) {
      const float* coordinates = mesh.tri_corner_coords(i, corner);
      int searchIndex = STLImport::findVertex(coordinates[0], coordinates[1], coordinates[2], verticesBuffer);
      if (searchIndex >= 0) {
        indices[corner] = searchIndex;
      } else {
        Lib3MF::sPosition vertex = {coordinates[0], coordinates[1], coordinates[2]};
        verticesBuffer->push_back(vertex);
        indices[corner] = vertexId;
        vertexId++;
      }
    }
    Lib3MF::sTriangle triangle = {indices[0], indices[1], indices[2]};
    triangleBuffer->push_back(triangle);
  }
}

int STLImport::findVertex(float x, float y, float z, std::vector<Lib3MF::sPosition>* verticesBuffer) {
  int index = 0;
  for (Lib3MF::sPosition vertex: *verticesBuffer) {
    if (x == vertex.m_Coordinates[0] && y == vertex.m_Coordinates[1] && z == vertex.m_Coordinates[2]) {
      return index;
    }
    index++;
  }
  return -1;
}
