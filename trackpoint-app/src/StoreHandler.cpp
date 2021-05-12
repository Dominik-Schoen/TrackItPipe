#include "StoreHandler.hpp"

void StoreHandler::addTrackingPoint(osg::Vec3 point, osg::Vec3 normal) {
  TrackPoint* trackPoint = new TrackPoint(point, normal);
  points.push_back(trackPoint);
  _root->addChild(trackPoint->getUppermostRoot());
}

StoreHandler::StoreHandler(osg::ref_ptr<osg::Group> root) {
  _root = root;
}

std::vector<TrackPoint*> StoreHandler::getPoints() {
  return points;
}
