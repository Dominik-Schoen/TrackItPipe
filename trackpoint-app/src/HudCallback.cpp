// Include own headers
#include "HudCallback.hpp"

// Include dependencies
#include <osg/MatrixTransform>

HudCallback::HudCallback(osg::Camera* camera) {
  _camera = camera;
}

void HudCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
  osg::MatrixTransform* pTM = dynamic_cast<osg::MatrixTransform*>(node);
	if (pTM) {
    osg::Camera* camera = _camera;
    osg::Vec3 translate = pTM->getMatrix().getTrans();
    osg::Vec3 scale = pTM->getMatrix().getScale();
    osg::Matrix mv = camera->getViewMatrix();

    osg::Matrix inv_mv = camera->getInverseViewMatrix();  // Convert the view matrix to a normal transformation matrix
    osg::Quat inv_q = inv_mv.getRotate();
    osg::Quat q = mv.getRotate();

    // The current rotating coordinate system of the model, and the coordinate system in which the camera is located, rotates clockwise 90 degrees around the x-axis.
    osg::Quat dq(osg::DegreesToRadians(90.0f), osg::Vec3(1.0f, 0.0f, 0.0f));
    static osg::Matrix mm = osg::Matrix::rotate(dq);
    mv.setTrans(translate);
    pTM->setMatrix(osg::Matrix::scale(scale) *  mv /** mm*/);
  }
}
