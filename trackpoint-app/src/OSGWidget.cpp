// Include own headers
#include "OSGWidget.hpp"
#include "PickHandler.hpp"

// Include dependencies
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
#include <osgUtil/MeshOptimizers>
#include <osg/PolygonMode>

#include <osgDB/WriteFile>
#include <osg/Material>
#include <osg/StateSet>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

namespace osgWidget {
  void Viewer::setupThreading() {
    if (_threadingModel == SingleThreaded) {
      if (_threadsRunning) {
        stopThreading();
      }
    } else {
      if (!_threadsRunning) {
        startThreading();
      }
    }
  }
}

void OSGWidget::fixMaterialState(osg::ref_ptr<osg::Node> node) {
  osg::StateSet* stateSet = node->getOrCreateStateSet();
  osg::Material* material = new osg::Material;

  material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

  stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
  stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
}

OSGWidget::OSGWidget(QWidget* parent): QOpenGLWidget(parent),
  graphicsWindow_(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height())),
  _viewer(new osgWidget::Viewer),
  selectionActive_(false),
  selectionFinished_(true)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Root node of the scene
  _root = new osg::Group;

  // Create the camera
  float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
  auto pixelRatio = this->devicePixelRatio();
  osg::Camera* camera = new osg::Camera;
  camera->setViewport(0, 0, this->width() * pixelRatio, this->height() * pixelRatio);
  camera->setClearColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
  camera->setProjectionMatrixAsPerspective(30.0f, aspectRatio, 1.0f, 1000.0f);
  camera->setGraphicsContext(graphicsWindow_);

  // Create the viewer
  _view = new osgViewer::View;
  _view->setCamera(camera);
  _view->setSceneData(_root);

  // Create coordinate axes
  _coordinateAxes = new osg::Geode;
  osg::ref_ptr<osg::ShapeDrawable> zAxis = new osg::ShapeDrawable();
  zAxis->setShape(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 0.1f, 100.0f));
  zAxis->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
  _coordinateAxes->addDrawable(zAxis.get());

  OSGWidget::fixMaterialState(_coordinateAxes);

  _root->addChild(_coordinateAxes);

  _axesNode = new osg::Geode;

  // Attach a manipulator (it's usually done for us when we use viewer.run())
  osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
  tm->setAllowThrow(false);
  _view->setCameraManipulator(tm);

  _viewer->addView(_view);
  _viewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
  _viewer->realize();

  _storeHandler = new StoreHandler(_root);
  _openScadRenderer = new OpenScadRenderer();
  _threeMFWriter = new ThreeMFWriter();

  osg::ref_ptr<PickHandler> picker = new PickHandler(_storeHandler, _openScadRenderer, _threeMFWriter, _axesNode);
  _root->addChild(picker->getOrCreateSelectionCylinder());

  _view->addEventHandler(picker.get());

  // This ensures that the widget will receive keyboard events. This focus
  // policy is not set by default. The default, Qt::NoFocus, will result in
  // keyboard events that are ignored.
  this->setFocusPolicy(Qt::StrongFocus);
  this->setMinimumSize(100, 100);

  // Ensures that the widget receives mouse move events even though no
  // mouse button has been pressed. We require this in order to let the
  // graphics window switch viewports properly.
  this->setMouseTracking(true);
}

OSGWidget::~OSGWidget() {
}

void OSGWidget::renderBaseMesh(const osg::ref_ptr<osg::Vec3Array> vertices, const osg::ref_ptr<osg::Vec3Array> normals) {
  _root->removeChild(_axesNode);

  osg::ref_ptr<osg::Geometry> meshGeometry = new osg::Geometry;
  meshGeometry->setVertexArray(vertices.get());
  meshGeometry->setNormalArray(normals.get(), osg::Array::BIND_PER_VERTEX);
  meshGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, vertices->getNumElements()));
  osgUtil::optimizeMesh(meshGeometry.get());
  _axesNode->addDrawable(meshGeometry.get());

  OSGWidget::fixMaterialState(_axesNode);

  _root->addChild(_axesNode);
}

void OSGWidget::paintEvent(QPaintEvent*) {
  this->makeCurrent();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  this->paintGL();

  painter.end();

  this->doneCurrent();
}

void OSGWidget::paintGL() {
  _viewer->frame();
}

void OSGWidget::resizeGL(int width, int height) {
  this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
  graphicsWindow_->resized(this->x(), this->y(), width, height);

  this->onResize(width, height);
}

void OSGWidget::keyPressEvent(QKeyEvent* event) {
  QString keyString = event->text();
  const char* keyData = keyString.toLocal8Bit().data();

  if (event->key() == Qt::Key_H) {
    this->onHome();
    return;
  }

  this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event) {
  QString keyString = event->text();
  const char* keyData = keyString.toLocal8Bit().data();

  this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keyData));
}

void OSGWidget::mouseMoveEvent(QMouseEvent* event) {
  auto pixelRatio = this->devicePixelRatio();
  this->getEventQueue()->mouseMotion(static_cast<float>(event->position().x() * pixelRatio), static_cast<float>(event->position().y() * pixelRatio));
}

void OSGWidget::mousePressEvent(QMouseEvent* event) {
  unsigned int button = 0;

  switch(event->button()) {
    case Qt::LeftButton:
      button = 1;
      break;

    case Qt::MiddleButton:
      button = 2;
      break;

    case Qt::RightButton:
      button = 3;
      break;

    default:
      break;
  }

  auto pixelRatio = this->devicePixelRatio();

  this->getEventQueue()->mouseButtonPress(static_cast<float>(event->position().x() * pixelRatio), static_cast<float>(event->position().y() * pixelRatio), button);
}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event) {
  unsigned int button = 0;

  switch(event->button()) {
    case Qt::LeftButton:
      button = 1;
      break;

    case Qt::MiddleButton:
      button = 2;
      break;

    case Qt::RightButton:
      button = 3;
      break;

    default:
      break;
  }

  auto pixelRatio = this->devicePixelRatio();

  this->getEventQueue()->mouseButtonRelease(static_cast<float>(pixelRatio * event->position().x()), static_cast<float>(pixelRatio * event->position().y()), button);
}

void OSGWidget::wheelEvent(QWheelEvent* event) {
  event->accept();
  int delta = event->angleDelta().y();

  osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;

  this->getEventQueue()->mouseScroll(motion);
}

bool OSGWidget::event(QEvent* event) {
  bool handled = QOpenGLWidget::event(event);

  // This ensures that the OSG widget is always going to be repainted after the
  // user performed some interaction. Doing this in the event handler ensures
  // that we don't forget about some event and prevents duplicate code.
  switch(event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
      this->update();
      break;

    case QEvent::Resize:
      this->onResize(this->width(), this->height());
      break;

    default:
      break;
  }

  return handled;
}

void OSGWidget::onHome() {
  osgViewer::ViewerBase::Views views;
  _viewer->getViews( views );

  for(std::size_t i = 0; i < views.size(); i++) {
    osgViewer::View* view = views.at(i);
    view->home();
  }
}

void OSGWidget::onResize(int width, int height) {
  std::vector<osg::Camera*> cameras;
  _viewer->getCameras(cameras);

  assert(cameras.size() == 1);

  auto pixelRatio = this->devicePixelRatio();

  cameras[0]->setViewport(0, 0, width * pixelRatio, height * pixelRatio);
}

osgGA::EventQueue* OSGWidget::getEventQueue() const {
  osgGA::EventQueue* eventQueue = graphicsWindow_->getEventQueue();
  if (eventQueue) {
    return eventQueue;
  } else {
    throw std::runtime_error("Unable to obtain valid event queue");
  }
}
