// Include own headers
#include "SteamVRTrackPoint.hpp"

// Include modules
#include "MeshTools.hpp"

SteamVRTrackPoint::SteamVRTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const float normalRotation, const bool compensation, const double length): TrackPoint(point, normal, normalModifier, normalRotation, compensation) {
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
  osg::Vec3 shift = _normal.operator*(_length + STEAMVR_THREAD_LENGTH + STEAMVR_ORIGIN_OFFSET);
  _trackOrigin = shift.operator+(_origin);
}

float SteamVRTrackPoint::getNormalCompensation() {
  float compensationLength = MeshTools::compensationLength(_normal, _normalModifier, STEAMVR_CONSTANT_RADIUS);
  return compensationLength > 0.0f ? compensationLength : 0.0f;
}
