#pragma once

#include "StoreHandler.hpp"
#include "OpenScadRenderer.hpp"
#include "ThreeMFWriter.hpp"

#include <osgGA/GUIEventHandler>

class PickHandler: public osgGA::GUIEventHandler {
public:
  PickHandler(StoreHandler* storeHandler, OpenScadRenderer* openScadRenderer, ThreeMFWriter* threeMFWriter, osg::ref_ptr<osg::Node> axesNode);
  osg::Node* getOrCreateSelectionCylinder();
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
  void moveTo(osg::Vec3f position);
  void rotateToNormalVector(osg::Vec3f normal);
  void setVisibility(bool mode);

protected:
  osg::ref_ptr<osg::Switch> _selectionSwitch;
  osg::ref_ptr<osg::MatrixTransform> _selectionTranslateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionRotateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionMoveToEndGroup;
  bool isSelection = true;

private:
  StoreHandler* _storeHandler;
  OpenScadRenderer* _openScadRenderer;
  ThreeMFWriter* _threeMFWriter;
  osg::ref_ptr<osg::Node> _axesNode;
};
