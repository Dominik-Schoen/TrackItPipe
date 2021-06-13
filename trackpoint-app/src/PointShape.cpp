// Include own headers
#include "PointShape.hpp"

// Include modules
#include "OSGWidget.hpp"
#include "defaults.hpp"
#include "resources.hpp"
#include "MeshTools.hpp"

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
  normal = normal.operator+(_normalModifier);
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
  if (_threadMesh) {
    _threadMesh->setColor(color);
  }
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
    _shape->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), STEAMVR_CONSTANT_RADIUS, steamVrTrackSettings.length));
    _shape->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    _geode->addDrawable(_shape.get());

    std::vector<Lib3MF::sPosition> verticesBuffer;
    std::vector<Lib3MF::sTriangle> triangleBuffer;
    for (unsigned int i = 0; i < sizeof(steamvrthread_VERTICES) / sizeof(float); i += 3) {
      Lib3MF::sPosition vertex = {steamvrthread_VERTICES[i], steamvrthread_VERTICES[i + 1], steamvrthread_VERTICES[i + 2]};
      verticesBuffer.push_back(vertex);
    }
    for (unsigned int i = 0; i < sizeof(steamvrthread_TRIANGLES) / sizeof(unsigned int); i += 3) {
      Lib3MF::sTriangle triangle = {steamvrthread_TRIANGLES[i], steamvrthread_TRIANGLES[i + 1], steamvrthread_TRIANGLES[i + 2]};
      triangleBuffer.push_back(triangle);
    }

    // Create osg style arrays
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    MeshTools::calculateNormals(verticesBuffer, triangleBuffer, vertices, normals);

    osg::ref_ptr<osg::Geode> thread = new osg::Geode;
    _threadMesh = new osg::Geometry;
    _threadMesh->setVertexArray(vertices.get());
    _threadMesh->setNormalArray(normals.get(), osg::Array::BIND_PER_VERTEX);
    _threadMesh->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vertices->getNumElements()));
    _threadMesh->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.2f));
    osgUtil::optimizeMesh(_threadMesh.get());

    thread->addDrawable(_threadMesh.get());
    OSGWidget::fixMaterialState(thread);
    _screwMove->addChild(thread.get());

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
