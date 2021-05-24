// Include own headers
#include "MainWindow.hpp"
#include "../gui/ui_MainWindow.h"

// Include dependencies
#include <QFileDialog>

MainWindow* globalPointer;

MainWindow* MainWindow::getInstance() {
  return globalPointer;
}

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow) {
  globalPointer = this;

  ui->setupUi(this);

  osgWidget = new OSGWidget(this);
  ui->sceneWidget->layout()->addWidget(osgWidget);

  // TODO: Add option for opening a project via double click
  //projectStore = new ProjectStore();
  renderView(NoMesh);
  projectStore.loadMesh("/home/johannes/Documents/GitLab/bachelorthesis/testdata/testbutton.3mf");
}

MainWindow::~MainWindow() {
  delete ui;
  delete osgWidget;
}

OSGWidget* MainWindow::getOsgWidget() {
  return osgWidget;
}

ProjectStore* MainWindow::getStore() {
  return &projectStore;
}

void MainWindow::renderView(GuiView view) {
  switch(view) {
    case NoMesh: {
      noMeshWidget = new NoMeshWidget(this);
      ui->controlWidget->layout()->addWidget(noMeshWidget);
      break;
    }
    case Edit: {
      editWidget = new EditWidget(this);
      ui->controlWidget->layout()->addWidget(editWidget);
      break;
    }
  }
}

void MainWindow::openFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a TrackpointApp project or a 3D-Object"), "", tr("TrackpointApp Project (*.trackproj);;3MF File (*.3mf);;STL File (*.stl)"));
}
