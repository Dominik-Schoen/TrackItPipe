#pragma once

// Include dependencies
#include "lib3mf_implicit.hpp"
#include <osg/Array>

class MeshTools {
public:
  static void calculateNormals(const std::vector<Lib3MF::sPosition> verticesBuffer, const std::vector<Lib3MF::sTriangle> triangleBuffer, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec3Array> normals);
};
