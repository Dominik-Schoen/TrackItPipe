// Include own headers
#include "TrackPoint.hpp"

// Include modules
#include "OSGWidget.hpp"

TrackPoint::TrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const float normalRotation, const bool compensation) {
  _origin = point;
  _normal = normal;
  _normalModifier = normalModifier;
  _normalRotation = normalRotation;
  _compensation = compensation;
}

osg::Vec3 TrackPoint::getTranslation() {
  return _origin;
}

osg::Vec3 TrackPoint::getRotation() {
  osg::Vec3 start = osg::Vec3(0.0f, 0.0f, 1.0f);
  osg::Matrix modifierRotation = osg::Matrix::rotate(_normalModifier.x() * M_PI / 180, osg::Vec3(1.0f, 0.0f, 0.0f), _normalModifier.y() * M_PI / 180, osg::Vec3(0.0f, 1.0f, 0.0f), _normalModifier.z() * M_PI / 180, osg::Vec3(0.0f, 0.0f, 1.0f));
  osg::Vec3 finalNormal = modifierRotation.preMult(_normal);

  // From https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  osg::Quat quat = osg::Quat(0.0f, 0.0f, 0.0f, 0.0f);
  quat.makeRotate(start, finalNormal);
  osg::Matrix matrix;
  quat.get(matrix);
  matrix = matrix.operator*(osg::Matrix::rotate(_normalRotation * M_PI / 180, finalNormal));
  quat.set(matrix);

  float sinr_cosp = 2 * (quat.w() * quat.x() + quat.y() * quat.z());
  float cosr_cosp = 1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y());
  float xRotation = std::atan2(sinr_cosp, cosr_cosp) * 180.0 / M_PI;

  float sinp = 2 * (quat.w() * quat.y() - quat.z() * quat.x());
  float yRotation;
  if (std::abs(sinp) >= 1) {
    yRotation = std::copysign(M_PI / 2, sinp) * 180.0 / M_PI;
  } else {
    yRotation = std::asin(sinp) * 180.0 / M_PI;
  }

  float siny_cosp = 2 * (quat.w() * quat.z() + quat.x() * quat.y());
  float cosy_cosp = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
  float zRotation = std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;

  return osg::Vec3(xRotation, yRotation, zRotation);
}

osg::Vec3 TrackPoint::getNormal() {
  return _normal;
}

osg::Vec3 TrackPoint::getNormalModifier() {
  return _normalModifier;
}

osg::Vec3 TrackPoint::getTrackPoint() {
  return _trackOrigin;
}

float TrackPoint::getNormalRotation() {
  return _normalRotation;
}

bool TrackPoint::getCompensation() {
  return _compensation;
}

void TrackPoint::updateNormalModifier(osg::Vec3 normalModifier) {
  _normalModifier = normalModifier;
}

void TrackPoint::updatePositions(osg::Vec3 origin) {
  _origin = origin;
}

void TrackPoint::updateNormalRotation(float normalRotation) {
  _normalRotation = normalRotation;
}

void TrackPoint::updateCompensation(bool compensation) {
  _compensation = compensation;
}
