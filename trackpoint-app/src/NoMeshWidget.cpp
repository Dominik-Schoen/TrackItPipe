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
}

NoMeshWidget::~NoMeshWidget() {
  delete ui;
}

void NoMeshWidget::loadMeshFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open a 3D-Object"), "", tr("3D Objects (*.3mf *.stl)"));
  MainWindow* parent = ((MainWindow*)this->parentWidget());
  parent->getProjectStorePointer()->loadMesh(fileName);
}
