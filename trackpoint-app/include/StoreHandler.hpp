#pragma once

#include "TrackPoint.hpp"

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Vec3>
#include <vector>

class StoreHandler {
public:
  void addTrackingPoint(osg::Vec3 point, osg::Vec3 normal);
  StoreHandler(osg::ref_ptr<osg::Group> root);
  std::vector<TrackPoint*> getPoints();

protected:
  std::vector<TrackPoint*> points;

private:
  osg::ref_ptr<osg::Group> _root;
};
