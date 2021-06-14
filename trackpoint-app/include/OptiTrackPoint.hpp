#pragma once

// Include modules
#include "defaults.hpp"
#include "TrackPoint.hpp"
#include "TrackSystemSettingsStructs.hpp"

class OptiTrackPoint: public TrackPoint {
public:
  OptiTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const double length = OPTITRACK_DEFAULT_LENGTH, const double radius = OPTITRACK_DEFAULT_RADIUS);
  double getLength();
  double getRadius();
  OptiTrackSettings getOptiTrackSettings();
  void updateOptiTrackSettings(OptiTrackSettings settings);

private:
  void updateShift();
  double _length;
  double _radius;
};
