#pragma once

#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/MatrixTransform>

class TrackPoint {
public:
  TrackPoint(osg::Vec3 point, osg::Vec3 normal);
  osg::ref_ptr<osg::MatrixTransform> getUppermostRoot();
  osg::Vec3 getTranslation();
  osg::Vec3 getRotation();
  osg::Vec3 getTrackPoint();

protected:
  osg::ref_ptr<osg::MatrixTransform> _translationGroup;
  osg::ref_ptr<osg::MatrixTransform> _rotationGroup;
  osg::ref_ptr<osg::MatrixTransform> _originFixGroup;
  osg::Vec3 _trackOrigin;

private:
  osg::Vec3 _origin;
  osg::Vec3 _normal;
};
