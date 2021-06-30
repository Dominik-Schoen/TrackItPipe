#pragma once

// Include modules
#include "TrackSystemSettingsStructs.hpp"
#include "enums.hpp"

// Include dependencies
#include <osg/Vec3>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Switch>
#include <osg/Geode>
#include "lib3mf_implicit.hpp"

class PointShape {
public:
  static void loadSteamvrThread();
  PointShape(const osg::ref_ptr<osg::Group> renderRoot, const ActiveTrackingSystem activeTrackingSystem, osg::Vec3f point, osg::Vec3f normal, osg::Vec3f normalModifier);
  ~PointShape();
  void moveTo(osg::Vec3f position);
  void setNormalModifier(osg::Vec3f normalModifier);
  void rotateToNormalVector(osg::Vec3f normal);
  void setVisibility(bool mode);
  void setColor(osg::Vec4 color);
  void setupOptiTrack(OptiTrackSettings optiTrackSettings);
  void setupEMFTrack(EMFTrackSettings emfTrackSettings);
  void setupSteamVRTrack(SteamVRTrackSettings steamVrTrackSettings);
  void setupActionPoints();
  osg::ref_ptr<osg::Geode> getMesh();

private:
  osg::Matrix emfYFix(osg::Vec3 normal);
  osg::ref_ptr<osg::Switch> _selectionSwitch;
  osg::ref_ptr<osg::MatrixTransform> _selectionTranslateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionRotateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionMoveToEndGroup;
  osg::ref_ptr<osg::MatrixTransform> _screwMove;
  osg::ref_ptr<osg::Group> _renderRoot;
  osg::ref_ptr<osg::ShapeDrawable> _shape;
  osg::ref_ptr<osg::ShapeDrawable> _optiConnector;
  osg::ref_ptr<osg::Geode> _thread;
  osg::ref_ptr<osg::Geode> _geode;
  osg::Vec3f _normalModifier;
  double _optiTrackSteamVRLength;
  ActiveTrackingSystem _activeTrackingSystem;
};
