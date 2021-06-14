// Include own headers
#include "OptiTrackPoint.hpp"

OptiTrackPoint::OptiTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const double length, const double radius): TrackPoint(point, normal, normalModifier) {
  _length = length;
  _radius = radius;
  updateShift();
}

double OptiTrackPoint::getLength() {
  return _length;
}

double OptiTrackPoint::getRadius() {
  return _radius;
}

OptiTrackSettings OptiTrackPoint::getOptiTrackSettings() {
  return OptiTrackSettings {_length, _radius};
}

void OptiTrackPoint::updateOptiTrackSettings(OptiTrackSettings settings) {
  _length = settings.length;
  _radius = settings.radius;
  updateShift();
}

void OptiTrackPoint::updateShift() {
  osg::Vec3 shift = _normal.operator*(_length);
  _trackOrigin = shift.operator+(_origin);
}
