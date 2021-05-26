#pragma once

#include "defaults.hpp"
#include "TrackPoint.hpp"

class OptiTrackPoint: TrackPoint {
public:
  OptiTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const double length = OPTITRACK_DEFAULT_LENGTH, const double radius = OPTITRACK_DEFAULT_RADIUS);
  double getLength();
  double getRadius();

private:
  double _length;
  double _radius;
};
