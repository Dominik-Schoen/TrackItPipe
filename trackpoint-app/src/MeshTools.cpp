// Include own header
#include "MeshTools.hpp"

// Include modules
#include "MainWindow.hpp"

// Include dependencies
#include <math.h>

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

float MeshTools::angleBetween(osg::Vec3 a, osg::Vec3 b) {
  float normA = a.length();
  float normB = b.length();
  float dotProduct = a.operator*(b);
  return acos(dotProduct / (normA * normB));
}

float MeshTools::compensationLength(osg::Vec3 a, osg::Vec3 b, float length) {
  osg::Matrix modifierRotation = osg::Matrix::rotate(b.x() * M_PI / 180, osg::Vec3(1.0f, 0.0f, 0.0f), b.y() * M_PI / 180, osg::Vec3(0.0f, 1.0f, 0.0f), b.z() * M_PI / 180, osg::Vec3(0.0f, 0.0f, 1.0f));
  osg::Vec3 newNormal = modifierRotation.preMult(a);
  newNormal.normalize();
  float angle = MeshTools::angleBetween(a, newNormal);
  return isnan(tan(angle) * length) ? 0.0f : tan(angle) * length;
}

bool MeshTools::optiTrackSanityCheck(const std::vector<OptiTrackPoint*> points, const bool showSuccessMessage) {
  // Check for three on a line
  bool foundLineProblem = false;
  if (points.size() >= 3) {
    for (int i = 0; i < (points.size() - 1); i++) {
      for (int j = i + 1; j < points.size(); j++) {
        for (int k = 0; k < points.size(); k++) {
          if (k != i && k != j) {
            osg::Vec3 a = points[i]->getTrackPoint();
            osg::Vec3 b = points[j]->getTrackPoint().operator-(a);
            osg::Vec3 point = points[k]->getTrackPoint();
            osg::Vec3 pMinusA = point.operator-(a);
            osg::Vec3 crossProduct = pMinusA.operator^(b);
            float distance = crossProduct.length() / b.length();
            if (distance < OPTITRACK_SANITY_DISTANCE_THRESHOLD) {
              foundLineProblem = true;
            }
          }
        }
      }
    }
  }
  // Check for four on a plane
  bool foundPlaneProblem = false;
  if (points.size() >= 4) {
    for (int i = 0; i < (points.size() - 2); i++) {
      for (int j = i + 1; j < (points.size() - 1); j++) {
        for (int k = j + 1; k < points.size(); k++) {
          for (int l = 0; l < points.size(); l++) {
            if (l != i && l != j && l != k) {
              osg::Vec3 a = points[i]->getTrackPoint();
              osg::Vec3 b = points[j]->getTrackPoint();
              osg::Vec3 c = points[k]->getTrackPoint();
              osg::Vec3 point = points[l]->getTrackPoint();
              osg::Plane* plane = new osg::Plane(a, b, c);
              float distance = std::abs(plane->distance(point));
              delete plane;
              if (distance < OPTITRACK_SANITY_DISTANCE_THRESHOLD) {
                foundPlaneProblem = true;
              }
            }
          }
        }
      }
    }
  }
  if (foundLineProblem) {
    MainWindow::getInstance()->showOptiTrackSanityLineError();
  }
  if (foundPlaneProblem) {
    MainWindow::getInstance()->showOptiTrackSanityPlaneError();
  }
  if (!foundLineProblem && !foundPlaneProblem && showSuccessMessage) {
    MainWindow::getInstance()->showOptiTrackSanitySuccess();
  }
  return foundLineProblem || foundPlaneProblem;
}
