// Include own headers
#include "PickHandler.hpp"

// Include modules
#include "TrackPointRenderer.hpp"

// Include dependencies
#include <osg/io_utils>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/StateSet>

#include <osgViewer/Viewer>

PickHandler::PickHandler(OSGWidget* osgWidget, osg::ref_ptr<osg::Group> root) {
  _osgWidget = osgWidget;

  _selectionSwitch = new osg::Switch;


  root->addChild(_selectionSwitch.get());
}

osg::Node* PickHandler::getPickerRoot() {
  return _selectionSwitch.get();
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
    if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH) {
      osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
      if (viewer) {
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv(intersector.get());
        iv.setTraversalMask(~0x1);
        viewer->getCamera()->accept(iv);
        if (intersector->containsIntersections()) {
          if (_addNewPoints) {
            for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
              if (std::find(result.nodePath.begin(), result.nodePath.end(), _osgWidget->getMesh()) != result.nodePath.end()) {
                _clickStartedOnElement = true;
                break;
              }
            }
          } else {
            bool found = false;
            for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
              if (found) break;
              for (PointShape* shape: _osgWidget->getPointRenderer()->getShapes()) {
                if (std::find(result.nodePath.begin(), result.nodePath.end(), shape->getMesh()) != result.nodePath.end()) {
                  _clickStartedOnElement = true;
                  found = true;
                  break;
                }
              }
            }
          }
        }
      }
    } else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && _clickStartedOnElement) {
      _clickStartedOnElement = false;
      osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
      if (viewer) {
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
        osgUtil::IntersectionVisitor iv(intersector.get());
        iv.setTraversalMask(~0x1);
        viewer->getCamera()->accept(iv);
        if (intersector->containsIntersections()) {
          if (_addNewPoints) {
            for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
              if (std::find(result.nodePath.begin(), result.nodePath.end(), _osgWidget->getMesh()) != result.nodePath.end()) {
                moveTo(result.localIntersectionPoint);
                rotateToNormalVector(result.localIntersectionNormal);
                addPoint(result.localIntersectionPoint, result.localIntersectionNormal);
                break;
              }
            }
          } else {
            bool found = false;
            for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
              if (found) break;
              int shapeId = 0;
              for (PointShape* shape: _osgWidget->getPointRenderer()->getShapes()) {
                if (std::find(result.nodePath.begin(), result.nodePath.end(), shape->getMesh()) != result.nodePath.end()) {
                  _selectedPoint = shapeId;
                  MainWindow::getInstance()->getEditWiget()->setSelection(shapeId);
                  found = true;
                  break;
                }
                shapeId++;
              }
            }
          }
        }
      }
    }
  }
  // Update position of picker on mouse move
  if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG) {
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer) {
      osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
      osgUtil::IntersectionVisitor iv(intersector.get());
      iv.setTraversalMask(~0x1);
      viewer->getCamera()->accept(iv);
      invalidateTrackPointColors();
      if (intersector->containsIntersections()) {
        if (_addNewPoints) {
          for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
            if (std::find(result.nodePath.begin(), result.nodePath.end(), _osgWidget->getMesh()) != result.nodePath.end()) {
              moveTo(result.localIntersectionPoint);
              rotateToNormalVector(result.localIntersectionNormal);
              setVisibility(true);
              break;
            }
          }
        } else {
          bool found = false;
          for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
            if (found) break;
            for (PointShape* shape: _osgWidget->getPointRenderer()->getShapes()) {
              if (std::find(result.nodePath.begin(), result.nodePath.end(), shape->getMesh()) != result.nodePath.end()) {
                shape->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f));
                found = true;
                break;
              }
            }
          }
        }
      } else {
        if (_addNewPoints) {
          MainWindow::getInstance()->getEditWiget()->invalidatePositions();
        }
        setVisibility(false);
      }
    }
  }
  return false;
}

void PickHandler::setSelection(bool addNewPoints) {
  _addNewPoints = addNewPoints;
  if (addNewPoints) {
    _selectedPoint = -1;
  }
  invalidateTrackPointColors();
  updateRenderer();
}

void PickHandler::updateRenderer() {
  delete _shape;
  ActiveTrackingSystem activeTrackingSystem = MainWindow::getInstance()->getEditWiget()->getSelectedTrackingSystem();
  switch(activeTrackingSystem) {
    case OptiTrack: {
      OptiTrackSettings settings = MainWindow::getInstance()->getStore()->getOptiTrackSettings();
      _optiTrackSteamVRLength = settings.length;
      _shape = new PointShape(_selectionSwitch, activeTrackingSystem, osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f));
      _shape->setupOptiTrack(settings);
      break;
    }
    case EMFTrack: {
      break;
    }
    case SteamVRTrack: {
      SteamVRTrackSettings settings = MainWindow::getInstance()->getStore()->getSteamVRTrackSettings();
      _optiTrackSteamVRLength = settings.length;
      _shape = new PointShape(_selectionSwitch, activeTrackingSystem, osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f));
      _shape->setupSteamVRTrack(settings);
      break;
    }
    case ActionPoints: {
      _shape = new PointShape(_selectionSwitch, activeTrackingSystem, osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 0.0f));
      _shape->setupActionPoints();
      break;
    }
  }
  if (_shape) {
    _shape->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.2f));
  }
  setVisibility(_addNewPoints);
}

void PickHandler::moveTo(osg::Vec3f position) {
  if (_shape) {
    _shape->moveTo(position);
    MainWindow::getInstance()->getEditWiget()->updatePositions(position);
  }
}

void PickHandler::rotateToNormalVector(osg::Vec3f normal) {
  MainWindow::getInstance()->getEditWiget()->updateNormals(normal);
  if (_shape) {
    osg::Vec3 modifier = MainWindow::getInstance()->getStore()->getNormalModifier();
    _shape->rotateToNormalVector(normal);
    _shape->setNormalModifier(modifier);
  }
}

void PickHandler::setVisibility(bool mode) {
  _selectionSwitch->setValue(0, mode);
}

void PickHandler::addPoint(osg::Vec3 point, osg::Vec3 normal) {
  ActiveTrackingSystem activeTrackingSystem = MainWindow::getInstance()->getEditWiget()->getSelectedTrackingSystem();
  MainWindow::getInstance()->getStore()->addTrackPoint(point, normal, activeTrackingSystem);
  _osgWidget->getPointRenderer()->render(MainWindow::getInstance()->getEditWiget()->getSelectedTrackingSystem());
}

void PickHandler::invalidateTrackPointColors() {
  int shapeId = 0;
  for (PointShape* shape: _osgWidget->getPointRenderer()->getShapes()) {
    if (_selectedPoint != shapeId) {
      shape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
    }
    shapeId++;
  }
}
