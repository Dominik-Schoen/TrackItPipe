// Include own header
#include "TrackPointRenderer.hpp"

// Include modules


// Include dependencies


TrackPointRenderer::TrackPointRenderer(osg::ref_ptr<osg::Group> renderRoot) {
  _renderRoot = renderRoot;
}

TrackPointRenderer::~TrackPointRenderer() {
}

void TrackPointRenderer::render(ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack: {
      std::vector<OptiTrackPoint*> points = MainWindow::getInstance()->getStore()->getOptiTrackPoints();
      break;
    }
    case EMFTrack: {
      break;
    }
    case SteamVRTrack: {
      break;
    }
  }
}
