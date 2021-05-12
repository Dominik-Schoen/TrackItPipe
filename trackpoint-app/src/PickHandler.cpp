#include "PickHandler.hpp"

#include <osg/io_utils>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/StateSet>

#include <osgViewer/Viewer>

PickHandler::PickHandler(StoreHandler* storeHandler, OpenScadRenderer* openScadRenderer, ThreeMFWriter* threeMFWriter, osg::ref_ptr<osg::Node> axesNode) {
  _storeHandler = storeHandler;
  _openScadRenderer = openScadRenderer;
  _threeMFWriter = threeMFWriter;
  _axesNode = axesNode;
}

osg::Node* PickHandler::getOrCreateSelectionCylinder() {
  if (!_selectionTranslateGroup) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::ShapeDrawable> cylinder = new osg::ShapeDrawable();
    cylinder->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f));
    cylinder->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.2f));
    geode->addDrawable(cylinder.get());

    {
      osg::StateSet* stateSet = geode->getOrCreateStateSet();
      osg::Material* material = new osg::Material;

      material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

      stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
      stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    }

    _selectionRotateGroup = new osg::MatrixTransform;
    _selectionRotateGroup->addChild(geode.get());

    _selectionMoveToEndGroup = new osg::MatrixTransform;
    _selectionMoveToEndGroup->addChild(_selectionRotateGroup.get());

    _selectionTranslateGroup = new osg::MatrixTransform;
    _selectionTranslateGroup->addChild(_selectionMoveToEndGroup.get());

    _selectionSwitch = new osg::Switch;
    _selectionSwitch->addChild(_selectionTranslateGroup.get());
  }
  return _selectionSwitch.get();
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  if (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) {
    isSelection = !isSelection;
    setVisibility(false);
  }
  if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && isSelection) {
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer) {
      osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
      osgUtil::IntersectionVisitor iv(intersector.get());
      iv.setTraversalMask(~0x1);
      viewer->getCamera()->accept(iv);
      if (intersector->containsIntersections()) {
        for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
          if (std::find(result.nodePath.begin(), result.nodePath.end(), _axesNode) != result.nodePath.end()) {
            moveTo(result.localIntersectionPoint);
            rotateToNormalVector(result.localIntersectionNormal);
            _storeHandler->addTrackingPoint(result.localIntersectionPoint, result.localIntersectionNormal);
            _openScadRenderer->render(_storeHandler->getPoints());
            _threeMFWriter->writeTrackPoints(_storeHandler->getPoints(), "/tmp/export.3mf");
            break;
          }
        }
      }
    }
  }
  if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && isSelection) {
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (viewer) {
      osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
      osgUtil::IntersectionVisitor iv(intersector.get());
      iv.setTraversalMask(~0x1);
      viewer->getCamera()->accept(iv);
      if (intersector->containsIntersections()) {
        for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
          if (std::find(result.nodePath.begin(), result.nodePath.end(), _axesNode) != result.nodePath.end()) {
            moveTo(result.localIntersectionPoint);
            rotateToNormalVector(result.localIntersectionNormal);
            setVisibility(true);
            break;
          }
        }
      } else {
        setVisibility(false);
      }
    }
  }
  return false;
}

void PickHandler::moveTo(osg::Vec3f position) {
  _selectionTranslateGroup->setMatrix(osg::Matrix::translate(position));
}

void PickHandler::rotateToNormalVector(osg::Vec3f normal) {
  _selectionRotateGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));
  osg::Vec3f movementVector = normal.operator*(5.0f);
  _selectionMoveToEndGroup->setMatrix(osg::Matrix::translate(movementVector));
}

void PickHandler::setVisibility(bool mode) {
  _selectionSwitch->setValue(0, mode);
}
