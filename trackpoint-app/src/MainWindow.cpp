#include "MainWindow.hpp"
#include "../gui/ui_MainWindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  osgWidget = new OSGWidget(this);
  ui->sceneWidget->layout()->addWidget(osgWidget);

  renderView(Edit);
}

MainWindow::~MainWindow() {
  delete ui;
  delete osgWidget;
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
