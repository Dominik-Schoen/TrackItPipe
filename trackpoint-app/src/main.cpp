#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osgGA/TrackballManipulator>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osg/PolygonMode>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "lib3mf_implicit.hpp"

const char* openScadBase =
  "$fn = 100;\n"
  "module optiTrackPointBase(translation, rotation) {\n"
  "translate(translation) rotate(rotation) cylinder(10, 1, 1, false);\n"
  "}\n";

class TrackPoint {
public:
  TrackPoint(osg::Vec3 point, osg::Vec3 normal);
  osg::ref_ptr<osg::MatrixTransform> getUppermostRoot();
  osg::Vec3 getTranslation();
  osg::Vec3 getRotation();

protected:
  osg::ref_ptr<osg::MatrixTransform> _translationGroup;
  osg::ref_ptr<osg::MatrixTransform> _rotationGroup;
  osg::ref_ptr<osg::MatrixTransform> _originFixGroup;

private:
  osg::Vec3 _origin;
  osg::Vec3 _normal;
  osg::Vec3 _trackOrigin;
};

TrackPoint::TrackPoint(osg::Vec3 point, osg::Vec3 normal) {
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  osg::ref_ptr<osg::ShapeDrawable> cylinder = new osg::ShapeDrawable();
  cylinder->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f));
  cylinder->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
  geode->addDrawable(cylinder.get());

  _rotationGroup = new osg::MatrixTransform;
  _rotationGroup->addChild(geode.get());
  _rotationGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));

  _originFixGroup = new osg::MatrixTransform;
  _originFixGroup->addChild(_rotationGroup.get());
  osg::Vec3f movementVector = normal.operator*(5.0f);
  _originFixGroup->setMatrix(osg::Matrix::translate(movementVector));

  _translationGroup = new osg::MatrixTransform;
  _translationGroup->addChild(_originFixGroup.get());
  _translationGroup->setMatrix(osg::Matrix::translate(point));

  _origin = point;
  _normal = normal;

  osg::Vec3 shift = normal.operator*(10.0f);
  _trackOrigin = shift.operator+(point);
  printf("TrackPoint is at %lf %lf %lf\n", _trackOrigin.x(), _trackOrigin.y(), _trackOrigin.z());
}

osg::ref_ptr<osg::MatrixTransform> TrackPoint::getUppermostRoot() {
  return _translationGroup.get();
}

osg::Vec3 TrackPoint::getTranslation() {
  return _origin;
}

osg::Vec3 TrackPoint::getRotation() {
  printf("YNorm: %lf %lf %lf\n", _normal.x(), _normal.y(), _normal.z());

  osg::Vec3 start = osg::Vec3(0.0f, 0.0f, 1.0f);

  // From https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  osg::Quat quat = osg::Quat(0.0f, 0.0f, 0.0f, 0.0f);
  quat.makeRotate(start, _normal);

  float sinr_cosp = 2 * (quat.w() * quat.x() + quat.y() * quat.z());
  float cosr_cosp = 1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y());
  float xRotation = std::atan2(sinr_cosp, cosr_cosp) * 180.0 / M_PI;

  float sinp = 2 * (quat.w() * quat.y() - quat.z() * quat.x());
  float yRotation;
  if (std::abs(sinp) >= 1) {
    yRotation = std::copysign(M_PI / 2, sinp) * 180.0 / M_PI;
  } else {
    yRotation = std::asin(sinp) * 180.0 / M_PI;
  }

  float siny_cosp = 2 * (quat.w() * quat.z() + quat.x() * quat.y());
  float cosy_cosp = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
  float zRotation = std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;

  return osg::Vec3(xRotation, yRotation, zRotation);
}

class ThreeMFWriter {
public:
  ThreeMFWriter();
  void writeTrackPoints(std::vector<TrackPoint*> points, std::string path);

private:
  Lib3MF::PWrapper _wrapper;
};

ThreeMFWriter::ThreeMFWriter() {
  _wrapper = Lib3MF::CWrapper::loadLibrary();
}

void ThreeMFWriter::writeTrackPoints(std::vector<TrackPoint*> points, std::string path) {
  // Load the file created by OpenSCAD
  Lib3MF::PModel model = _wrapper->CreateModel();
  Lib3MF::PReader reader = model->QueryReader("3mf");
  reader->ReadFromFile("/tmp/output.3mf");

  Lib3MF::PMetaDataGroup metaData = model->GetMetaDataGroup();
  printf("Having %d MetaData entries\n", metaData->GetMetaDataCount());

  Lib3MF::PWriter writer = model->QueryWriter("3mf");
  writer->WriteToFile(path);
}

class OpenScadRenderer {
public:
  void render(std::vector<TrackPoint*> points);
};

void OpenScadRenderer::render(std::vector<TrackPoint*> points) {
  std::ofstream scadFile;
  scadFile.open("/tmp/output.scad");
  scadFile << openScadBase;
  scadFile << "import(\"testbutton.stl\");\n";
  for (TrackPoint* point: points) {
    osg::Vec3 translation = point->getTranslation();
    osg::Vec3 rotation = point->getRotation();
    scadFile << "optiTrackPointBase([" << translation.x() << "," << translation.y() << "," << translation.z() << "], [" << rotation.x() << "," << rotation.y() << "," << rotation.z() << "]);\n";
  }
  scadFile.close();
  system("openscad -o /tmp/output.3mf /tmp/output.scad");
}

class StoreHandler {
public:
  void addTrackingPoint(osg::Vec3 point, osg::Vec3 normal);
  StoreHandler(osg::ref_ptr<osg::Group> root);
  std::vector<TrackPoint*> getPoints();

protected:
  std::vector<TrackPoint*> points;

private:
  osg::ref_ptr<osg::Group> _root;
};

void StoreHandler::addTrackingPoint(osg::Vec3 point, osg::Vec3 normal) {
  TrackPoint* trackPoint = new TrackPoint(point, normal);
  points.push_back(trackPoint);
  _root->addChild(trackPoint->getUppermostRoot());
}

StoreHandler::StoreHandler(osg::ref_ptr<osg::Group> root) {
  _root = root;
}

std::vector<TrackPoint*> StoreHandler::getPoints() {
  return points;
}

StoreHandler* storeHandler;
OpenScadRenderer* openScadRenderer;
ThreeMFWriter* threeMFWriter;
osg::ref_ptr<osg::Node> axesNode;

class PickHandler: public osgGA::GUIEventHandler {
public:
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
  bool isSelection = false;
};

osg::Node* PickHandler::getOrCreateSelectionCylinder() {
  if (!_selectionTranslateGroup) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::ShapeDrawable> cylinder = new osg::ShapeDrawable();
    cylinder->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 10.0f));
    cylinder->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 0.2f));
    geode->addDrawable(cylinder.get());

    _selectionRotateGroup = new osg::MatrixTransform;
    _selectionRotateGroup->addChild(geode.get());

    _selectionMoveToEndGroup = new osg::MatrixTransform;
    _selectionMoveToEndGroup->addChild(_selectionRotateGroup.get());

    _selectionTranslateGroup = new osg::MatrixTransform;
    _selectionTranslateGroup->addChild(_selectionMoveToEndGroup.get());

    _selectionSwitch = new osg::Switch;
    _selectionSwitch->addChild(_selectionTranslateGroup.get());
  }
  return _selectionSwitch.get();
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  if (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) {
    isSelection = !isSelection;
    setVisibility(false);
  }
  if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && isSelection) {
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
    if (viewer) {
      osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
      osgUtil::IntersectionVisitor iv(intersector.get());
      iv.setTraversalMask(~0x1);
      viewer->getCamera()->accept(iv);
      if (intersector->containsIntersections()) {
        for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
          if (std::find(result.nodePath.begin(), result.nodePath.end(), axesNode) != result.nodePath.end()) {
            moveTo(result.localIntersectionPoint);
            rotateToNormalVector(result.localIntersectionNormal);
            storeHandler->addTrackingPoint(result.localIntersectionPoint, result.localIntersectionNormal);
            openScadRenderer->render(storeHandler->getPoints());
            threeMFWriter->writeTrackPoints(storeHandler->getPoints(), "/tmp/export.3mf");
            break;
          }
        }
      }
    }
  }
  if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && isSelection) {
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
    if (viewer) {
      osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
      osgUtil::IntersectionVisitor iv(intersector.get());
      iv.setTraversalMask(~0x1);
      viewer->getCamera()->accept(iv);
      if (intersector->containsIntersections()) {
        for (const osgUtil::LineSegmentIntersector::Intersection result: intersector->getIntersections()) {
          if (std::find(result.nodePath.begin(), result.nodePath.end(), axesNode) != result.nodePath.end()) {
            moveTo(result.localIntersectionPoint);
            rotateToNormalVector(result.localIntersectionNormal);
            setVisibility(true);
            break;
          }
        }
      } else {
        setVisibility(false);
      }
    }
  }
  return false;
}

void PickHandler::moveTo(osg::Vec3f position) {
  _selectionTranslateGroup->setMatrix(osg::Matrix::translate(position));
}

void PickHandler::rotateToNormalVector(osg::Vec3f normal) {
  _selectionRotateGroup->setMatrix(osg::Matrix::rotate(osg::Vec3f(0.0f, 0.0f, 1.0f), normal));
  osg::Vec3f movementVector = normal.operator*(5.0f);
  _selectionMoveToEndGroup->setMatrix(osg::Matrix::translate(movementVector));
}

void PickHandler::setVisibility(bool mode) {
  _selectionSwitch->setValue(0, mode);
}

int main(int argc, char** argv) {
    // Root node of the scene
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // Create the viewer
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    viewer.realize();

    // Add axesNode under root
    axesNode = osgDB::readNodeFile("../../testdata/testbutton.stl");
    if (!axesNode) {
        printf("Origin node not loaded, model not found\n");
        return 1;
    }
    root->addChild(axesNode);

    // Attach a manipulator (it's usually done for us when we use viewer.run())
    osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
    viewer.setCameraManipulator(tm);

    storeHandler = new StoreHandler(root);
    openScadRenderer = new OpenScadRenderer();
    threeMFWriter = new ThreeMFWriter();

    osg::ref_ptr<PickHandler> picker = new PickHandler();
    root->addChild(picker->getOrCreateSelectionCylinder());

    viewer.addEventHandler(picker.get());

    return viewer.run();
}
