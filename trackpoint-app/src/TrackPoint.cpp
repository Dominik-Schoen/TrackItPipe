// Include own headers
#include "TrackPoint.hpp"

// Include modules
#include "OSGWidget.hpp"

TrackPoint::TrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier) {
  _origin = point;
  _normal = normal;
  _normalModifier = normalModifier;
}

osg::Vec3 TrackPoint::getTranslation() {
  return _origin;
}

osg::Vec3 TrackPoint::getRotation() {
  osg::Vec3 start = osg::Vec3(0.0f, 0.0f, 1.0f);

  // From https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  osg::Quat quat = osg::Quat(0.0f, 0.0f, 0.0f, 0.0f);
  quat.makeRotate(start, _normal);

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

void TrackPoint::updateNormalModifier(osg::Vec3 normalModifier) {
  _normalModifier = normalModifier;
}
