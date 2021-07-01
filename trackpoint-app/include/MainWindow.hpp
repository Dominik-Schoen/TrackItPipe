#pragma once

// Include modules
#include "OSGWidget.hpp"
#include "NoMeshWidget.hpp"
#include "EditWidget.hpp"
#include "ProjectStore.hpp"
#include "enums.hpp"

// Include dependencies
#include <QMainWindow>
#include <QCloseEvent>

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
  void showErrorMessage(std::string message, std::string title = "Error");

protected:
  virtual void closeEvent(QCloseEvent *event);

private:
  void newFile();
  void load();
  bool save();
  bool saveAs();
  void close();
  void cleanup();
  bool saveChangesPopup();
  bool isRenderingPopup();
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
