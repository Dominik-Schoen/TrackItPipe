#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osgGA/TrackballManipulator>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osg/PolygonMode>

class PickHandler: public osgGA::GUIEventHandler {
public:
  osg::Node* getOrCreateSelectionCylinder();
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
  void moveTo(osg::Vec3f position);
  void rotateToNormalVector(osg::Vec3f normal);

protected:
  osg::ref_ptr<osg::MatrixTransform> _selectionTranslateGroup;
  osg::ref_ptr<osg::MatrixTransform> _selectionRotateGroup;
  //osg::ref_ptr<osg::> _selectionTransformation;
};

osg::Node* PickHandler::getOrCreateSelectionCylinder() {
  if (!_selectionTranslateGroup) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f)));

    _selectionRotateGroup = new osg::MatrixTransform;
    _selectionRotateGroup->addChild(geode.get());

    _selectionTranslateGroup = new osg::MatrixTransform;
    _selectionTranslateGroup->addChild(_selectionRotateGroup.get());

    osg::StateSet* ss = _selectionTranslateGroup->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
  }
  return _selectionTranslateGroup.get();
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE || ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON || !(ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL)) {
    return false;
  }
  osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
  if (viewer) {
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
    osgUtil::IntersectionVisitor iv(intersector.get());
    iv.setTraversalMask(~0x1);
    viewer->getCamera()->accept(iv);
    if (intersector->containsIntersections()) {
      osgUtil::LineSegmentIntersector::Intersection result = *(intersector->getIntersections().begin());
      moveTo(result.localIntersectionPoint);
      rotateToNormalVector(result.localIntersectionNormal);
    }
  }
  return false;
}

void PickHandler::moveTo(osg::Vec3f position) {
  _selectionTranslateGroup->setMatrix(osg::Matrix::translate(position));
}

void PickHandler::rotateToNormalVector(osg::Vec3f normal) {
  _selectionRotateGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));
}

int main(int argc, char** argv) {
    // Root node of the scene
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    viewer.realize();

    // Add axesNode under root
    osg::ref_ptr<osg::Node> axesNode = osgDB::readNodeFile("../../testdata/zPlate_0.stl");
    if (!axesNode) {
        printf("Origin node not loaded, model not found\n");
        return 1;
    }
    root->addChild(axesNode);

    // Attach a manipulator (it's usually done for us when we use viewer.run())
    osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
    viewer.setCameraManipulator(tm);

    osg::ref_ptr<PickHandler> picker = new PickHandler();
    root->addChild(picker->getOrCreateSelectionCylinder());

    viewer.addEventHandler(picker.get());

    return viewer.run();
}
