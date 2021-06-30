#pragma once

// Include modules
#include "defaults.hpp"
#include "TrackPoint.hpp"
#include "TrackSystemSettingsStructs.hpp"

class EMFTrackPoint: public TrackPoint {
public:
  EMFTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const float normalRotation, const bool compensation, const double width = EMFTRACK_DEFAULT_WIDTH, const double height = EMFTRACK_DEFAULT_HEIGHT, const double depth = EMFTRACK_DEFAULT_DEPTH);
  double getWidth();
  double getHeight();
  double getDepth();
  EMFTrackSettings getEMFTrackSettings();
  void updateEMFTrackSettings(EMFTrackSettings settings);

private:
  double _width;
  double _height;
  double _depth;
};
