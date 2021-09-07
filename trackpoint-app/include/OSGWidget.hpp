#pragma once

// Include dependencies
#include <QPoint>
#include <QOpenGLWidget>

#include <osg/ref_ptr>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

namespace osgWidget {
  //! The subclass of osgViewer::CompositeViewer we use
  /*!
   * This subclassing allows us to remove the annoying automatic
   * setting of the CPU affinity to core 0 by osgViewer::ViewerBase,
   * osgViewer::CompositeViewer's base class.
   */
  class Viewer: public osgViewer::CompositeViewer {
    public:
	    virtual void setupThreading();
  };
}

class PickHandler; // Forward declaration
class TrackPointRenderer; // Forward declaration

class OSGWidget : public QOpenGLWidget {
  Q_OBJECT

public:
  static void fixMaterialState(osg::ref_ptr<osg::Node> node, osg::Vec4* color = nullptr);
  OSGWidget(QWidget* parent = nullptr);
  virtual ~OSGWidget();
  void renderBaseMesh(const osg::ref_ptr<osg::Vec3Array> vertices, const osg::ref_ptr<osg::Vec3Array> normals);
  osg::ref_ptr<osg::Geode> getMesh();
  osg::ref_ptr<osg::Group> getVerifyGroup();
  PickHandler* getPicker();
  TrackPointRenderer* getPointRenderer();
  void loadSteamvrThread();
  void loadSteamvrCollision();
  void clear();
  osg::ref_ptr<osg::Geometry> _steamvrThreadMesh;
  osg::ref_ptr<osg::Geometry> _steamvrTrackerMesh;

protected:
  virtual void paintEvent(QPaintEvent* paintEvent);
  virtual void paintGL();
  virtual void resizeGL(int width, int height);

  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);

  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

  virtual bool event(QEvent* event);

private:
  virtual void onHome();
  virtual void onResize(int width, int height);

  osg::ref_ptr<osg::Group> createAxesPreview();

  osgGA::EventQueue* getEventQueue() const;
  PickHandler* _picker;
  TrackPointRenderer* _pointRenderer;

  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
  osg::ref_ptr<osgWidget::Viewer> _viewer;
  osgViewer::View* _view;
  osg::ref_ptr<osg::Group> _root;
  osg::ref_ptr<osg::Group> _pointRoot;
  osg::ref_ptr<osg::Group> _verifyGroup;
  osg::ref_ptr<osg::Group> _coordinateAxes;
  osg::ref_ptr<osg::Geode> _mesh;

  QPoint selectionStart_;
  QPoint selectionEnd_;

  bool selectionActive_;
  bool selectionFinished_;

  bool _steamvrLoaded = false;
  bool _steamvrCollisionLoaded = false;
};
