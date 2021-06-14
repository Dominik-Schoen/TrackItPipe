#pragma once

// Include modules
#include "OSGWidget.hpp"
#include "NoMeshWidget.hpp"
#include "EditWidget.hpp"
#include "ProjectStore.hpp"
#include "enums.hpp"

// Include dependencies
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  static MainWindow* getInstance();
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  void renderView(GuiView view);
  OSGWidget* getOsgWidget();
  ProjectStore* getStore();
  EditWidget* getEditWiget();

private:
  void newFile();
  void load();
  bool save();
  bool saveAs();
  void close();
  void cleanup();
  bool saveChangesPopup();
  Ui::MainWindow* ui;
  OSGWidget* osgWidget;
  NoMeshWidget* noMeshWidget;
  EditWidget* editWidget;
  ProjectStore* projectStore;
  QMenu* fileMenu;
  QAction* newAct;
  QAction* loadAct;
  QAction* saveAct;
  QAction* saveAsAct;
  QAction* closeAct;
};
