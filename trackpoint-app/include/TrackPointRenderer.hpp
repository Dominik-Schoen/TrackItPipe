#pragma once

// Include modules
#include "enums.hpp"
#include "OSGWidget.hpp"
#include "PointShape.hpp"
#include "TrackPoint.hpp"

// Include dependencies
#include <osg/Group>

class TrackPointRenderer {
public:
  TrackPointRenderer(OSGWidget* osgWidget, osg::ref_ptr<osg::Group> renderRoot);
  ~TrackPointRenderer();
  void render(ActiveTrackingSystem activeTrackingSystem);
  std::vector<PointShape*> getShapes();
  void clear();

private:
  PointShape* addPointShape(TrackPoint* point, ActiveTrackingSystem activeTrackingSystem);
  void commonSetupPointShape(PointShape* shape, TrackPoint* point, int id);
  OSGWidget* _osgWidget;
  osg::ref_ptr<osg::Group> _renderRoot;
  std::vector<PointShape*> _shapes;
};
