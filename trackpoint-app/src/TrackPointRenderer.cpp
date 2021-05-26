// Include own header
#include "TrackPointRenderer.hpp"

// Include modules
#include "OptiTrackPoint.hpp"
#include "MainWindow.hpp"

// Include dependencies


TrackPointRenderer::TrackPointRenderer(OSGWidget* osgWidget, osg::ref_ptr<osg::Group> renderRoot) {
  _osgWidget = osgWidget;
  _renderRoot = renderRoot;
}

TrackPointRenderer::~TrackPointRenderer() {
}

void TrackPointRenderer::render(ActiveTrackingSystem activeTrackingSystem) {
  switch(activeTrackingSystem) {
    case OptiTrack: {
      std::vector<OptiTrackPoint*> points = MainWindow::getInstance()->getStore()->getOptiTrackPoints();
      _shapes.clear();
      for (OptiTrackPoint* point: points) {
        PointShape* newShape = new PointShape(_renderRoot, activeTrackingSystem, point->getTranslation(), point->getNormal(), point->getNormalModifier());
        newShape->setupOptiTrack(point->getOptiTrackSettings());
        newShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
        newShape->rotateToNormalVector(point->getNormal());
        _shapes.push_back(newShape);
      }
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
