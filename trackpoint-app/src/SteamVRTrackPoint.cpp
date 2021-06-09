// Include own headers
#include "SteamVRTrackPoint.hpp"

SteamVRTrackPoint::SteamVRTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const double length): TrackPoint(point, normal, normalModifier) {
  _length = length;

  osg::Vec3 shift = normal.operator*(length);
  _trackOrigin = shift.operator+(point);
}

double SteamVRTrackPoint::getLength() {
  return _length;
}

SteamVRTrackSettings SteamVRTrackPoint::getSteamVRTrackSettings() {
  return SteamVRTrackSettings {_length};
}

void SteamVRTrackPoint::updateSteamVRTrackSettings(SteamVRTrackSettings settings) {
  _length = settings.length;
}
