// Include own headers
#include "OptiTrackPoint.hpp"

OptiTrackPoint::OptiTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const double length, const double radius): TrackPoint(point, normal) {
  _length = length;
  _radius = radius;

  osg::Vec3 shift = normal.operator*(10.0f);
  _trackOrigin = shift.operator+(point);
}

double OptiTrackPoint::getLength() {
  return _length;
}

double OptiTrackPoint::getRadius() {
  return _radius;
}
