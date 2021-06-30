// Include own headers
#include "PointShape.hpp"

// Include modules
#include "OSGWidget.hpp"
#include "defaults.hpp"
#include "MainWindow.hpp"

// Include dependencies
#include <osgUtil/MeshOptimizers>
#include "lib3mf_implicit.hpp"

PointShape::PointShape(const osg::ref_ptr<osg::Group> renderRoot, const ActiveTrackingSystem activeTrackingSystem, osg::Vec3f point, osg::Vec3f normal, osg::Vec3f normalModifier) {
  _renderRoot = renderRoot;
  _activeTrackingSystem = activeTrackingSystem;

  _screwMove = new osg::MatrixTransform;

  _selectionRotateGroup = new osg::MatrixTransform;
  _selectionRotateGroup->addChild(_screwMove.get());

  _selectionMoveToEndGroup = new osg::MatrixTransform;
  _selectionMoveToEndGroup->addChild(_selectionRotateGroup.get());

  _selectionTranslateGroup = new osg::MatrixTransform;
  _selectionTranslateGroup->addChild(_selectionMoveToEndGroup.get());

  _selectionSwitch = new osg::Switch;
  _selectionSwitch->addChild(_selectionTranslateGroup.get());

  _renderRoot->addChild(_selectionSwitch.get());

  moveTo(point);
  setNormalModifier(normalModifier);
  rotateToNormalVector(normal);
}

PointShape::~PointShape() {
  _renderRoot->removeChild(_selectionSwitch.get());
}

void PointShape::moveTo(osg::Vec3f position) {
  _selectionTranslateGroup->setMatrix(osg::Matrix::translate(position));
}

void PointShape::setNormalModifier(osg::Vec3f normalModifier) {
  _normalModifier = normalModifier;
}

void PointShape::rotateToNormalVector(osg::Vec3f normal) {
  osg::Matrix modifierRotation = osg::Matrix::rotate(_normalModifier.x() * M_PI / 180, osg::Vec3(1.0f, 0.0f, 0.0f), _normalModifier.y() * M_PI / 180, osg::Vec3(0.0f, 1.0f, 0.0f), _normalModifier.z() * M_PI / 180, osg::Vec3(0.0f, 0.0f, 1.0f));
  normal = modifierRotation.preMult(normal);
  normal.normalize();
  _selectionRotateGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));
  if (_activeTrackingSystem == OptiTrack || _activeTrackingSystem == SteamVRTrack) {
    osg::Vec3f movementVector = normal.operator*(_optiTrackSteamVRLength / 2);
    _selectionMoveToEndGroup->setMatrix(osg::Matrix::translate(movementVector));
  }
  if (_activeTrackingSystem == SteamVRTrack) {
    osg::Vec3f movementVector = osg::Vec3f(0.0f, 0.0f, 1.0f).operator*(_optiTrackSteamVRLength / 2);
    _screwMove->setMatrix(osg::Matrix::translate(movementVector));
  }
}

void PointShape::setVisibility(bool mode) {
  _selectionSwitch->setValue(0, mode);
}

void PointShape::setColor(osg::Vec4 color) {
  _shape->setColor(color);
  if (_thread) {
    OSGWidget::fixMaterialState(_thread, &color);
  }
  if (_optiConnector) {
    _optiConnector->setColor(color);
  }
}

void PointShape::setupOptiTrack(OptiTrackSettings optiTrackSettings) {
  if (_activeTrackingSystem == OptiTrack) {
    _optiTrackSteamVRLength = optiTrackSettings.length;
    _geode = new osg::Geode;
    _shape = new osg::ShapeDrawable;
    _shape->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, -2.5f), optiTrackSettings.radius, optiTrackSettings.length - 5.0f));
    _shape->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_shape.get());
    _optiConnector = new osg::ShapeDrawable;
    _optiConnector->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, optiTrackSettings.length - 7.5f), 0.74f, 5.0f));
    _optiConnector->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_optiConnector.get());
    OSGWidget::fixMaterialState(_geode);
    _selectionRotateGroup->addChild(_geode.get());
  }
}

void PointShape::setupEMFTrack(EMFTrackSettings emfTrackSettings) {
  if (_activeTrackingSystem == EMFTrack) {
    _geode = new osg::Geode;
    _shape = new osg::ShapeDrawable;
    _shape->setShape(new osg::Box(osg::Vec3(0.0f, 0.0f, static_cast<float>(emfTrackSettings.depth) / 2), static_cast<float>(emfTrackSettings.width), static_cast<float>(emfTrackSettings.height), static_cast<float>(emfTrackSettings.depth)));
    _shape->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_shape.get());
    OSGWidget::fixMaterialState(_geode);
    _selectionRotateGroup->addChild(_geode.get());
  }
}

void PointShape::setupSteamVRTrack(SteamVRTrackSettings steamVrTrackSettings) {
  if (_activeTrackingSystem == SteamVRTrack) {
    _optiTrackSteamVRLength = steamVrTrackSettings.length;
    _geode = new osg::Geode;
    _shape = new osg::ShapeDrawable;
    _shape->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), STEAMVR_CONSTANT_RADIUS, steamVrTrackSettings.length));
    _shape->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_shape.get());

    MainWindow::getInstance()->getOsgWidget()->loadSteamvrThread();
    _thread = new osg::Geode;
    _thread->addDrawable(MainWindow::getInstance()->getOsgWidget()->_steamvrThreadMesh.get());
    OSGWidget::fixMaterialState(_thread);
    _screwMove->addChild(_thread.get());

    OSGWidget::fixMaterialState(_geode);
    _selectionRotateGroup->addChild(_geode.get());
  }
}

void PointShape::setupActionPoints() {
  if (_activeTrackingSystem == ActionPoints) {
    _geode = new osg::Geode;
    _shape = new osg::ShapeDrawable();
    _shape->setShape(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.5f));
    _shape->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_shape.get());
    OSGWidget::fixMaterialState(_geode);
    _selectionRotateGroup->addChild(_geode.get());
  }
}

osg::ref_ptr<osg::Geode> PointShape::getMesh() {
  return _geode;
}

osg::Matrix PointShape::emfYFix(osg::Vec3 normal) {
  osg::Vec3 xyNormal = osg::Vec3(normal.x(), normal.y(), 0.0f);
  xyNormal.normalize();
  float deg = (90.0f * M_PI / 180) * normal.x() * -1;
  return osg::Matrix::rotate(deg, normal);
}
