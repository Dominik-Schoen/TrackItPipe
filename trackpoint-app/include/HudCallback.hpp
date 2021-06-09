#pragma once

// Include dependencies
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/Camera>

class HudCallback: public osg::NodeCallback {
public:
  HudCallback(osg::Camera* camera);//: m_viewer(viewer);
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
  osg::Camera* _camera;
};
