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

  _selectionRotateGroup = new osg::MatrixTransform;

  _selectionMoveToEndGroup = new osg::MatrixTransform;
  _selectionMoveToEndGroup->addChild(_selectionRotateGroup.get());

  _selectionTranslateGroup = new osg::MatrixTransform;
  _selectionTranslateGroup->addChild(_selectionMoveToEndGroup.get());

  _selectionSwitch = new osg::Switch;
  _selectionSwitch->addChild(_selectionTranslateGroup.get());

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
          for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
            if (std::find(result.nodePath.begin(), result.nodePath.end(), _osgWidget->getMesh()) != result.nodePath.end()) {
              moveTo(result.localIntersectionPoint);
              rotateToNormalVector(result.localIntersectionNormal);
              addPoint(result.localIntersectionPoint, result.localIntersectionNormal);
              break;
            }
            /*for () {

            }*/
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
        MainWindow::getInstance()->getEditWiget()->invalidatePositions();
        setVisibility(false);
      }
    }
  }
  return false;
}

void PickHandler::setTrackingSystem(ActiveTrackingSystem activeTrackingSystem) {
  _activeTrackingSystem = activeTrackingSystem;
  updateRenderer();
}

void PickHandler::setSelection(bool addNewPoints) {
  _addNewPoints = addNewPoints;
  updateRenderer();
}

void PickHandler::moveTo(osg::Vec3f position) {
  _selectionTranslateGroup->setMatrix(osg::Matrix::translate(position));
  MainWindow::getInstance()->getEditWiget()->updatePositions(position);
}

void PickHandler::rotateToNormalVector(osg::Vec3f normal) {
  MainWindow::getInstance()->getEditWiget()->updateNormals(normal);
  osg::Vec3 modifier = MainWindow::getInstance()->getStore()->getNormalModifier();
  normal = normal.operator+(modifier);
  normal.normalize();
  _selectionRotateGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));
  osg::Vec3f movementVector = normal.operator*(_optiTrackSteamVRLength / 2);
  _selectionMoveToEndGroup->setMatrix(osg::Matrix::translate(movementVector));
}

void PickHandler::updateRenderer() {
  removeAllShapes();
  switch(_activeTrackingSystem) {
    case OptiTrack: {
      OptiTrackSettings settings = MainWindow::getInstance()->getStore()->getOptiTrackSettings();
      _optiTrackSteamVRLength = settings.length;
      osg::ref_ptr<osg::Geode> geode = new osg::Geode;
      osg::ref_ptr<osg::ShapeDrawable> cylinder = new osg::ShapeDrawable();
      cylinder->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), settings.radius, settings.length));
      cylinder->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.2f));
      geode->addDrawable(cylinder.get());
      OSGWidget::fixMaterialState(geode);
      _selectionRotateGroup->addChild(geode.get());
      break;
    }
    case EMFTrack: {
      break;
    }
    case SteamVRTrack: {
      break;
    }
  }
  setVisibility(_addNewPoints);
}

void PickHandler::removeAllShapes() {
  _selectionRotateGroup->removeChildren(0, _selectionRotateGroup->getNumChildren());
}

void PickHandler::setVisibility(bool mode) {
  _selectionSwitch->setValue(0, mode);
}

void PickHandler::addPoint(osg::Vec3 point, osg::Vec3 normal) {
  switch(_activeTrackingSystem) {
    case OptiTrack: {
      MainWindow::getInstance()->getStore()->addOptiTrackPoint(point, normal);
      _osgWidget->getPointRenderer()->render(MainWindow::getInstance()->getEditWiget()->getSelectedTrackingSystem());
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

void PickHandler::invalidateTrackPointColors() {
  for (PointShape* shape: _osgWidget->getPointRenderer()->getShapes()) {
    shape->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.2f));
  }
}
