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
  static MainWindow* getInstance();
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  void renderView(GuiView view);
  OSGWidget* getOsgWidget();
  ProjectStore* getStore();

private:
  void openFile();
  Ui::MainWindow* ui;
  OSGWidget* osgWidget;
  NoMeshWidget* noMeshWidget;
  EditWidget* editWidget;
  ProjectStore* projectStore;
};
