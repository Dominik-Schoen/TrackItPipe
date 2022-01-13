#pragma once

// Include modules
#include "defaults.hpp"
#include "TrackPoint.hpp"
#include "TrackSystemSettingsStructs.hpp"

class SteamVRTrackPoint: public TrackPoint {
public:
  SteamVRTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const float normalRotation, const bool compensation, const double length = STEAMVR_DEFAULT_LENGTH);
  double getLength();
  SteamVRTrackSettings getSteamVRTrackSettings();
  void updateSteamVRTrackSettings(SteamVRTrackSettings settings);
  float getNormalCompensation();
  static int getMinimumRequiredPoints();

private:
  void updateShift();
  double _length;
  static const int _minimumPointsRequired = 1;
};
