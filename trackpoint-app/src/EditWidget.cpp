#include "EditWidget.hpp"
#include "../gui/ui_EditWidget.h"

EditWidget::EditWidget(QWidget* parent): QWidget(parent), ui(new Ui::EditWidget) {
  ui->setupUi(this);
}

EditWidget::~EditWidget() {
  delete ui;
}
