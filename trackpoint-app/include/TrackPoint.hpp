#pragma once

#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/MatrixTransform>

class TrackPoint {
public:
  TrackPoint(const osg::Vec3 point, const osg::Vec3 normal, const osg::Vec3 normalModifier);
  osg::Vec3 getTranslation();
  osg::Vec3 getRotation();
  osg::Vec3 getNormal();
  osg::Vec3 getNormalModifier();
  osg::Vec3 getTrackPoint();
  void updateNormalModifier(osg::Vec3 normalModifier);

protected:
  osg::ref_ptr<osg::MatrixTransform> _translationGroup;
  osg::ref_ptr<osg::MatrixTransform> _rotationGroup;
  osg::ref_ptr<osg::MatrixTransform> _originFixGroup;
  osg::Vec3 _trackOrigin;

private:
  osg::Vec3 _origin;
  osg::Vec3 _normal;
  osg::Vec3 _normalModifier;
};
