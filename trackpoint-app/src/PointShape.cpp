// Include own headers
#include "PointShape.hpp"

// Include modules
#include "OSGWidget.hpp"

// Include dependencies
#include <osgDB/ReadFile>

PointShape::PointShape(const osg::ref_ptr<osg::Group> renderRoot, const ActiveTrackingSystem activeTrackingSystem, osg::Vec3f point, osg::Vec3f normal, osg::Vec3f normalModifier) {
  _renderRoot = renderRoot;
  _activeTrackingSystem = activeTrackingSystem;

  _selectionRotateGroup = new osg::MatrixTransform;

  _screwMove = new osg::MatrixTransform;

  _selectionMoveToEndGroup = new osg::MatrixTransform;
  _selectionMoveToEndGroup->addChild(_selectionRotateGroup.get());
  _selectionMoveToEndGroup->addChild(_screwMove.get());

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
  normal = normal.operator+(_normalModifier);
  normal.normalize();
  _selectionRotateGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));
  if (_activeTrackingSystem == OptiTrack || _activeTrackingSystem == SteamVRTrack) {
    osg::Vec3f movementVector = normal.operator*(_optiTrackSteamVRLength / 2);
    _selectionMoveToEndGroup->setMatrix(osg::Matrix::translate(movementVector));
  }
  if (_activeTrackingSystem == SteamVRTrack) {
    osg::Vec3f movementVector = normal.operator*(_optiTrackSteamVRLength);
    _screwMove->setMatrix(osg::Matrix::translate(movementVector));
  }
}

void PointShape::setVisibility(bool mode) {
  _selectionSwitch->setValue(0, mode);
}

void PointShape::setColor(osg::Vec4 color) {
  _shape->setColor(color);
}

void PointShape::setupOptiTrack(OptiTrackSettings optiTrackSettings) {
  if (_activeTrackingSystem == OptiTrack) {
    _optiTrackSteamVRLength = optiTrackSettings.length;
    _geode = new osg::Geode;
    _shape = new osg::ShapeDrawable();
    _shape->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), optiTrackSettings.radius, optiTrackSettings.length));
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
    _shape = new osg::ShapeDrawable();
    _shape->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, steamVrTrackSettings.length));
    _shape->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_shape.get());
    osg::ref_ptr<osg::Node> screw = osgDB::readNodeFile("../resources/steamvrthread.stl");
    _screwMove->addChild(screw.get());
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
