#pragma once

// Include modules
#include "MainWindow.hpp"
#include "OSGWidget.hpp"
#include "PointShape.hpp"

// Include dependencies
#include <osgGA/GUIEventHandler>

class PickHandler: public osgGA::GUIEventHandler {
public:
  PickHandler(OSGWidget* osgWidget, osg::ref_ptr<osg::Group> root);
  osg::Node* getPickerRoot();
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
  void setTrackingSystem(ActiveTrackingSystem activeTrackingSystem);
  void setSelection(bool addNewPoints);
  void updateRenderer();

//protected:

private:
  osg::ref_ptr<osg::Switch> _selectionSwitch;
  osg::ref_ptr<osg::MatrixTransform> _selectionTranslateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionRotateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionMoveToEndGroup;
  ActiveTrackingSystem _activeTrackingSystem = OptiTrack;
  bool _addNewPoints = true;
  bool _clickStartedOnElement = false;
  OSGWidget* _osgWidget;
  double _optiTrackSteamVRLength;
  void moveTo(osg::Vec3f position);
  void rotateToNormalVector(osg::Vec3f normal);
  void removeAllShapes();
  void setVisibility(bool mode);
  void addPoint(osg::Vec3 point, osg::Vec3 normal);
  void invalidateTrackPointColors();
};
