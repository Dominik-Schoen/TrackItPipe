#include "NoMeshWidget.hpp"
#include "../gui/ui_NoMeshWidget.h"

NoMeshWidget::NoMeshWidget(QWidget* parent): QWidget(parent), ui(new Ui::NoMeshWidget) {
  ui->setupUi(this);
}

NoMeshWidget::~NoMeshWidget() {
  delete ui;
}
