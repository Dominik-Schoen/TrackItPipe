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

protected:
  osg::ref_ptr<osg::MatrixTransform> _selectionCylinder;
};

osg::Node* PickHandler::getOrCreateSelectionCylinder() {
  if (!_selectionCylinder) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f)));

    _selectionCylinder = new osg::MatrixTransform;
    _selectionCylinder->setNodeMask(0.1);
    _selectionCylinder->addChild(geode.get());

    osg::StateSet* ss = _selectionCylinder->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
  }
  return _selectionCylinder.get();
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
      printf("Found intersection at: %lf, %lf, %lf\n", result.localIntersectionPoint.x(), result.localIntersectionPoint.y(), result.localIntersectionPoint.z());
      printf("Intersection normals: %lf, %lf, %lf\n", result.localIntersectionNormal.x(), result.localIntersectionNormal.y(), result.localIntersectionNormal.z());
    }
  }
  return false;
}

int main(int argc, char** argv) {
    // Root node of the scene
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    viewer.realize();

    // Add a small sphere
    osg::ref_ptr<osg::ShapeDrawable> mouseSphere = new osg::ShapeDrawable;
    mouseSphere->setShape(new osg::Sphere(osg::Vec3(3.0f, 0.0f, 0.0f), 1.0f));
    mouseSphere->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

    osg::ref_ptr<osg::Geode> mouseParent = new osg::Geode;
    mouseParent->addDrawable(mouseSphere.get());
    root->addChild(mouseParent);

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
