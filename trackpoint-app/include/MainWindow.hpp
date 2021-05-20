#pragma once

#include "OSGWidget.hpp"
#include "NoMeshWidget.hpp"
#include "EditWidget.hpp"
#include "ProjectStore.hpp"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum GuiView {
  NoMesh,
  Edit,
};

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  void renderView(GuiView view);
  ProjectStore* getProjectStorePointer();
  OSGWidget* getOsgWidget();

private:
  void openFile();
  Ui::MainWindow* ui;
  ProjectStore* projectStore;
  OSGWidget* osgWidget;
  NoMeshWidget* noMeshWidget;
  EditWidget* editWidget;
};
