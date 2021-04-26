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
  osg::Node* getOrCreateSelectionPoint();
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
  osg::ref_ptr<osg::MatrixTransform> _selectionPoint;
};

osg::Node* PickHandler::getOrCreateSelectionPoint() {
  if (!_selectionPoint) {
    printf("Got here\n");
  }
  return _selectionPoint.get();
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

int main(int argc, char** argv)
{
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
    if (!axesNode)
    {
        printf("Origin node not loaded, model not found\n");
        return 1;
    }
    root->addChild(axesNode);

    // Attach a manipulator (it's usually done for us when we use viewer.run())
    osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
    viewer.setCameraManipulator(tm);

    osg::ref_ptr<PickHandler> picker = new PickHandler();
    viewer.addEventHandler(picker.get());

    return viewer.run();
}
