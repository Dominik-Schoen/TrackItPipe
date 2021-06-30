// Include own headers
#include "EMFTrackPoint.hpp"

EMFTrackPoint::EMFTrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier, const float normalRotation, const bool compensation, const double width, const double height, const double depth): TrackPoint(point, normal, normalModifier, normalRotation, compensation) {
  _width = width;
  _height = height;
  _depth = depth;
}

double EMFTrackPoint::getWidth() {
  return _width;
}

double EMFTrackPoint::getHeight() {
  return _height;
}

double EMFTrackPoint::getDepth() {
  return _depth;
}

EMFTrackSettings EMFTrackPoint::getEMFTrackSettings() {
  return EMFTrackSettings {_width, _height, _depth};
}

void EMFTrackPoint::updateEMFTrackSettings(EMFTrackSettings settings) {
  _width = settings.width;
  _height = settings.height;
  _depth = settings.depth;
}
