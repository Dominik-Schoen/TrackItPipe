// Include own headers
#include "SteamVRTrackPoint.hpp"

SteamVRTrackPoint::SteamVRTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const double length): TrackPoint(point, normal, normalModifier) {
  _length = length;
  updateShift();
}

double SteamVRTrackPoint::getLength() {
  return _length;
}

SteamVRTrackSettings SteamVRTrackPoint::getSteamVRTrackSettings() {
  return SteamVRTrackSettings {_length};
}

void SteamVRTrackPoint::updateSteamVRTrackSettings(SteamVRTrackSettings settings) {
  _length = settings.length;
  updateShift();
}

void SteamVRTrackPoint::updateShift() {
  osg::Vec3 shift = _normal.operator*(_length);
  _trackOrigin = shift.operator+(_origin);
}
