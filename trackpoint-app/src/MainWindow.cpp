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

  noMeshWidget = new NoMeshWidget();
  editWidget = new EditWidget();

  // TODO: Add option for opening a project via double click
  projectStore = new ProjectStore();
  renderView(NoMesh);

  // Setup menu
  newAct = new QAction(tr("&New project"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  //connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

  loadAct = new QAction(tr("&Load project"), this);
  loadAct->setShortcuts(QKeySequence::Open);
  loadAct->setStatusTip(tr("Load an existing project"));
  connect(loadAct, &QAction::triggered, this, &MainWindow::load);

  saveAct = new QAction(tr("&Save project"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Update the project file"));
  connect(saveAct, &QAction::triggered, this, &MainWindow::save);

  saveAsAct = new QAction(tr("&Save project as"), this);
  saveAsAct->setStatusTip(tr("Save the project in a new file"));
  connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

  closeAct = new QAction(tr("&Close project"), this);
  closeAct->setShortcuts(QKeySequence::Close);
  closeAct->setStatusTip(tr("Closing the current project"));
  //connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addSeparator();
  fileMenu->addAction(loadAct);
  fileMenu->addSeparator();
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(closeAct);
}

MainWindow::~MainWindow() {
  delete ui;
  delete osgWidget;
}

OSGWidget* MainWindow::getOsgWidget() {
  return osgWidget;
}

ProjectStore* MainWindow::getStore() {
  return projectStore;
}

EditWidget* MainWindow::getEditWiget() {
  return editWidget;
}

void MainWindow::renderView(GuiView view) {
  switch(view) {
    case NoMesh: {
      editWidget->setParent(nullptr);
      ui->controlWidget->layout()->addWidget(noMeshWidget);
      break;
    }
    case Edit: {
      noMeshWidget->setParent(nullptr);
      ui->controlWidget->layout()->addWidget(editWidget);
      break;
    }
  }
}

void MainWindow::load() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a TrackpointApp Project"), "", tr("TrackpointApp Projects (*.trackproj)"));
  std::string projectFile = fileName.toUtf8().constData();
  MainWindow::getInstance()->getStore()->loadProject(projectFile);
}

void MainWindow::save() {
  if (!projectStore->saveProject()) {
    saveAs();
  }
}

void MainWindow::saveAs() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save your TrackpointApp Project"), "", tr("TrackpointApp Projects (*.trackproj)"));
  std::string fileString = fileName.toUtf8().constData();
  if (!projectStore->saveProject(fileString)) {
    // TODO: Show error popup
  }
}

void MainWindow::loadStaticMeshes() {
  
}
