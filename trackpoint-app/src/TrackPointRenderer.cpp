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
  clear();
  switch(activeTrackingSystem) {
    case OptiTrack: {
      std::vector<OptiTrackPoint*> points = MainWindow::getInstance()->getStore()->getOptiTrackPoints();
      int id = 0;
      for (OptiTrackPoint* point: points) {
        PointShape* newShape = new PointShape(_renderRoot, activeTrackingSystem, point->getTranslation(), point->getNormal(), point->getNormalModifier());
        newShape->setupOptiTrack(point->getOptiTrackSettings());
        if (id == MainWindow::getInstance()->getEditWiget()->getSelectedPoint()) {
          newShape->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f));
        } else {
          newShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
        }
        newShape->rotateToNormalVector(point->getNormal());
        _shapes.push_back(newShape);
        id++;
      }
      break;
    }
    case EMFTrack: {
      break;
    }
    case SteamVRTrack: {
      std::vector<SteamVRTrackPoint*> points = MainWindow::getInstance()->getStore()->getSteamVRTrackPoints();
      int id = 0;
      for (SteamVRTrackPoint* point: points) {
        PointShape* newShape = new PointShape(_renderRoot, activeTrackingSystem, point->getTranslation(), point->getNormal(), point->getNormalModifier());
        newShape->setupSteamVRTrack(point->getSteamVRTrackSettings());
        if (id == MainWindow::getInstance()->getEditWiget()->getSelectedPoint()) {
          newShape->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f));
        } else {
          newShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
        }
        newShape->rotateToNormalVector(point->getNormal());
        _shapes.push_back(newShape);
        id++;
      }
      break;
    }
    case ActionPoints: {
      std::vector<ActionPoint*> points = MainWindow::getInstance()->getStore()->getActionPoints();
      int id = 0;
      for (ActionPoint* point: points) {
        PointShape* newShape = new PointShape(_renderRoot, activeTrackingSystem, point->getTranslation(), point->getNormal(), point->getNormalModifier());
        newShape->setupActionPoints();
        if (id == MainWindow::getInstance()->getEditWiget()->getSelectedPoint()) {
          newShape->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f));
        } else {
          newShape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
        }
        newShape->rotateToNormalVector(point->getNormal());
        _shapes.push_back(newShape);
        id++;
      }
      break;
    }
  }
  _osgWidget->update();
}

std::vector<PointShape*> TrackPointRenderer::getShapes() {
  return _shapes;
}

void TrackPointRenderer::clear() {
  for (PointShape* shape: _shapes) {
    delete shape;
  }
  _shapes.clear();
}
