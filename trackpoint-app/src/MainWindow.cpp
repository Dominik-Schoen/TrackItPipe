#include "MainWindow.hpp"
#include "../gui/ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  osgWidget = new OSGWidget(nullptr);
  ui->sceneWidget->layout()->addWidget(osgWidget);
}

MainWindow::~MainWindow()
{
  delete ui;
}
