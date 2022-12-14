// Include own headers
#include "NoMeshWidget.hpp"
#include "../gui/ui_NoMeshWidget.h"

// Include modules
#include "MainWindow.hpp"

// Include dependencies
#include <QFileDialog>

NoMeshWidget::NoMeshWidget(QWidget* parent): QWidget(parent), ui(new Ui::NoMeshWidget) {
  ui->setupUi(this);
  QObject::connect(ui->noMeshButton, &QPushButton::clicked, this, &NoMeshWidget::loadMeshFile);
  QObject::connect(ui->loadProjectButton, &QPushButton::clicked, this, &NoMeshWidget::loadProjectFile);
}

NoMeshWidget::~NoMeshWidget() {
  delete ui;
}

void NoMeshWidget::loadMeshFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a 3D-Object"), "", tr("3D Objects (*.3mf *.stl)"));
  std::string meshFile = fileName.toUtf8().constData();
  MainWindow::getInstance()->getStore()->loadMesh(meshFile);
}

void NoMeshWidget::loadProjectFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a TrackpointApp Project"), "", tr("TrackpointApp Projects (*.trackproj)"));
  std::string projectFile = fileName.toUtf8().constData();
  MainWindow::getInstance()->getStore()->loadProject(projectFile);
}
