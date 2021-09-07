#pragma once

// Include modules
#include "OptiTrackPoint.hpp"

// Include dependencies
#include "lib3mf_implicit.hpp"
#include <osg/Array>

class MeshTools {
public:
  static void calculateNormals(const std::vector<Lib3MF::sPosition> verticesBuffer, const std::vector<Lib3MF::sTriangle> triangleBuffer, osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec3Array> normals);
  static float angleBetween(osg::Vec3 a, osg::Vec3 b);
  static float compensationLength(osg::Vec3 a, osg::Vec3 b, float length);
  static bool optiTrackSanityCheck(const std::vector<OptiTrackPoint*> points, const bool showSuccessMessage);
};
