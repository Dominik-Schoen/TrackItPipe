// Include own header
#include "MeshTools.hpp"

void MeshTools::calculateNormals(const std::vector<Lib3MF::sPosition> verticesBuffer, const std::vector<Lib3MF::sTriangle> triangleBuffer, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec3Array> normals) {
  for (const Lib3MF::sTriangle triangle: triangleBuffer) {
    // Create osg vectors
    osg::Vec3 vertex[3] = {};
    for (char i = 0; i < 3; i++) {
      vertex[i] = osg::Vec3(verticesBuffer[triangle.m_Indices[i]].m_Coordinates[0], verticesBuffer[triangle.m_Indices[i]].m_Coordinates[1], verticesBuffer[triangle.m_Indices[i]].m_Coordinates[2]);
    }
    // Calculate normal
    osg::Vec3 edgeOne = vertex[1].operator-(vertex[0]);
    osg::Vec3 edgeTwo = vertex[2].operator-(vertex[0]);
    osg::Vec3 normal = edgeOne.operator^(edgeTwo);
    normal.normalize();
    // Store them
    for (int i = 0; i < 3; i++) {
      vertices->push_back(vertex[i]);
      normals->push_back(normal);
    }
  }
}
