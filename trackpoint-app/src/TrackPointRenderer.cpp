// Include own header
#include "TrackPointRenderer.hpp"

// Include modules
#include "MainWindow.hpp"


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
        PointShape* newShape = addPointShape(static_cast<TrackPoint*>(point), activeTrackingSystem);
        newShape->setCompensation(point->getCompensation(), point->getNormalCompensation());
        newShape->setupOptiTrack(point->getOptiTrackSettings());
        commonSetupPointShape(newShape, static_cast<TrackPoint*>(point), id);
        id++;
      }
      break;
    }
    case EMFTrack: {
      std::vector<EMFTrackPoint*> points = MainWindow::getInstance()->getStore()->getEMFTrackPoints();
      int id = 0;
      for (EMFTrackPoint* point: points) {
        PointShape* newShape = addPointShape(static_cast<TrackPoint*>(point), activeTrackingSystem);
        newShape->setCompensation(point->getCompensation(), point->getNormalCompensation());
        newShape->setupEMFTrack(point->getEMFTrackSettings());
        commonSetupPointShape(newShape, static_cast<TrackPoint*>(point), id);
        id++;
      }
      break;
    }
    case SteamVRTrack: {
      std::vector<SteamVRTrackPoint*> points = MainWindow::getInstance()->getStore()->getSteamVRTrackPoints();
      int id = 0;
      for (SteamVRTrackPoint* point: points) {
        PointShape* newShape = addPointShape(static_cast<TrackPoint*>(point), activeTrackingSystem);
        newShape->setCompensation(point->getCompensation(), point->getNormalCompensation());
        newShape->setupSteamVRTrack(point->getSteamVRTrackSettings());
        commonSetupPointShape(newShape, static_cast<TrackPoint*>(point), id);
        id++;
      }
      break;
    }
    case ActionPoints: {
      std::vector<ActionPoint*> points = MainWindow::getInstance()->getStore()->getActionPoints();
      int id = 0;
      for (ActionPoint* point: points) {
        PointShape* newShape = addPointShape(static_cast<TrackPoint*>(point), activeTrackingSystem);
        newShape->setCompensation(point->getCompensation(), 0.0f);
        newShape->setupActionPoints();
        commonSetupPointShape(newShape, static_cast<TrackPoint*>(point), id);
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

PointShape* TrackPointRenderer::addPointShape(TrackPoint* point, ActiveTrackingSystem activeTrackingSystem) {
  PointShape* newShape = new PointShape(_renderRoot, activeTrackingSystem, point->getTranslation(), point->getNormal(), point->getNormalModifier(), point->getNormalRotation());
  return newShape;
}

void TrackPointRenderer::commonSetupPointShape(PointShape* shape, TrackPoint* point, int id) {
  if (id == MainWindow::getInstance()->getEditWiget()->getSelectedPoint()) {
    shape->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f));
  } else {
    shape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
  }
  shape->rotateToNormalVector(point->getNormal(), point->getNormalRotation());
  _shapes.push_back(shape);
}
