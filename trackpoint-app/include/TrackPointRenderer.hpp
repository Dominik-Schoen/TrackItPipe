#pragma once

// Include modules
#include "enums.hpp"

// Include dependencies
#include <osg/Group>

class TrackPointRenderer {
public:
  TrackPointRenderer(osg::ref_ptr<osg::Group> renderRoot);
  ~TrackPointRenderer();
  void render(ActiveTrackingSystem activeTrackingSystem);

private:
  osg::ref_ptr<osg::Group> _renderRoot;
};
