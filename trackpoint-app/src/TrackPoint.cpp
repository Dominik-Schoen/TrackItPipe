// Include own headers
#include "TrackPoint.hpp"

// Include dependencies
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/StateSet>

TrackPoint::TrackPoint(osg::Vec3 point, osg::Vec3 normal) {
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  osg::ref_ptr<osg::ShapeDrawable> cylinder = new osg::ShapeDrawable();
  cylinder->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f));
  cylinder->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
  geode->addDrawable(cylinder.get());

  {
    osg::StateSet* stateSet = geode->getOrCreateStateSet();
    osg::Material* material = new osg::Material;

    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  }

  _rotationGroup = new osg::MatrixTransform;
  _rotationGroup->addChild(geode.get());
  _rotationGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));

  _originFixGroup = new osg::MatrixTransform;
  _originFixGroup->addChild(_rotationGroup.get());
  osg::Vec3f movementVector = normal.operator*(5.0f);
  _originFixGroup->setMatrix(osg::Matrix::translate(movementVector));

  _translationGroup = new osg::MatrixTransform;
  _translationGroup->addChild(_originFixGroup.get());
  _translationGroup->setMatrix(osg::Matrix::translate(point));

  _origin = point;
  _normal = normal;

  osg::Vec3 shift = normal.operator*(10.0f);
  _trackOrigin = shift.operator+(point);
  printf("TrackPoint is at %lf %lf %lf\n", _trackOrigin.x(), _trackOrigin.y(), _trackOrigin.z());
}

osg::ref_ptr<osg::MatrixTransform> TrackPoint::getUppermostRoot() {
  return _translationGroup.get();
}

osg::Vec3 TrackPoint::getTranslation() {
  return _origin;
}

osg::Vec3 TrackPoint::getRotation() {
  printf("YNorm: %lf %lf %lf\n", _normal.x(), _normal.y(), _normal.z());

  osg::Vec3 start = osg::Vec3(0.0f, 0.0f, 1.0f);

  // From https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  osg::Quat quat = osg::Quat(0.0f, 0.0f, 0.0f, 0.0f);
  quat.makeRotate(start, _normal);

  float sinr_cosp = 2 * (quat.w() * quat.x() + quat.y() * quat.z());
  float cosr_cosp = 1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y());
  float xRotation = std::atan2(sinr_cosp, cosr_cosp) * 180.0 / M_PI;

  float sinp = 2 * (quat.w() * quat.y() - quat.z() * quat.x());
  float yRotation;
  if (std::abs(sinp) >= 1) {
    yRotation = std::copysign(M_PI / 2, sinp) * 180.0 / M_PI;
  } else {
    yRotation = std::asin(sinp) * 180.0 / M_PI;
  }

  float siny_cosp = 2 * (quat.w() * quat.z() + quat.x() * quat.y());
  float cosy_cosp = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
  float zRotation = std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;

  return osg::Vec3(xRotation, yRotation, zRotation);
}

osg::Vec3 TrackPoint::getTrackPoint() {
  return _trackOrigin;
}
