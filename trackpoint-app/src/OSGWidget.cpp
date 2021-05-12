#include "OSGWidget.hpp"
#include "PickHandler.hpp"

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

  // Add axesNode under root
  _axesNode = osgDB::readNodeFile("../../testdata/testbutton.stl");
  if (!_axesNode) {
      printf("Origin node not loaded, model not found\n");
  }

  // Set material for basic lighting and enable depth tests. Else, the sphere
  // will suffer from rendering errors.
  {
    osg::StateSet* stateSet = _axesNode->getOrCreateStateSet();
    osg::Material* material = new osg::Material;

    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  }

  _root->addChild(_axesNode);

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

void OSGWidget::keyPressEvent( QKeyEvent* event )
{
  QString keyString   = event->text();
  const char* keyData = keyString.toLocal8Bit().data();

  if( event->key() == Qt::Key_S )
  {

    // Further processing is required for the statistics handler here, so we do
    // not return right away.
  }
  else if( event->key() == Qt::Key_D )
  {
    osgDB::writeNodeFile( *_viewer->getView(0)->getSceneData(),
                          "/tmp/sceneGraph.osg" );

    return;
  }
  else if( event->key() == Qt::Key_H )
  {
    this->onHome();
    return;
  }

  this->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::keyReleaseEvent( QKeyEvent* event )
{
  QString keyString   = event->text();
  const char* keyData = keyString.toLocal8Bit().data();

  this->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KeySymbol( *keyData ) );
}

void OSGWidget::mouseMoveEvent( QMouseEvent* event )
{
  // Note that we have to check the buttons mask in order to see whether the
  // left button has been pressed. A call to `button()` will only result in
  // `Qt::NoButton` for mouse move events.
  /*if( selectionActive_ && event->buttons() & Qt::LeftButton )
  {
    selectionEnd_ = event->pos();

    // Ensures that new paint events are created while the user moves the
    // mouse.
    this->update();
  }
  else
  {*/
    auto pixelRatio = this->devicePixelRatio();

    this->getEventQueue()->mouseMotion( static_cast<float>( event->x() * pixelRatio ),
                                        static_cast<float>( event->y() * pixelRatio ) );
  //}
}

void OSGWidget::mousePressEvent( QMouseEvent* event )
{
  // Selection processing
  /*if( selectionActive_ && event->button() == Qt::LeftButton )
  {
    selectionStart_    = event->pos();
    selectionEnd_      = selectionStart_; // Deletes the old selection
    selectionFinished_ = false;           // As long as this is set, the rectangle will be drawn
  }

  // Normal processing
  else
  {*/
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch( event->button() )
    {
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

    this->getEventQueue()->mouseButtonPress( static_cast<float>( event->x() * pixelRatio ),
                                             static_cast<float>( event->y() * pixelRatio ),
                                             button );
    //}
}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event)
{
  // Selection processing: Store end position and obtain selected objects
  // through polytope intersection.
  /*if( selectionActive_ && event->button() == Qt::LeftButton )
  {
    selectionEnd_      = event->pos();
    selectionFinished_ = true; // Will force the painter to stop drawing the
                               // selection rectangle
  }

  // Normal processing
  else
  {*/
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button

    unsigned int button = 0;

    switch( event->button() )
    {
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

    this->getEventQueue()->mouseButtonRelease( static_cast<float>( pixelRatio * event->x() ),
                                               static_cast<float>( pixelRatio * event->y() ),
                                               button );
  //}
}

void OSGWidget::wheelEvent( QWheelEvent* event )
{
  // Ignore wheel events as long as the selection is active.
  if( selectionActive_ )
    return;

  event->accept();
  int delta = event->angleDelta().y();

  osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                               : osgGA::GUIEventAdapter::SCROLL_DOWN;

  this->getEventQueue()->mouseScroll( motion );
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
