// Include own headers
#include "EditWidget.hpp"
#include "../gui/ui_EditWidget.h"

// Include modules
#include "OSGWidget.hpp"
#include "PickHandler.hpp"

EditWidget::EditWidget(QWidget* parent): QWidget(parent), ui(new Ui::EditWidget) {
  ui->setupUi(this);
  ui->insertionToolButton->setCheckable(true);
  ui->insertionToolButton->setChecked(true);
  QObject::connect(ui->insertionToolButton, &QToolButton::clicked, this, [=](){ this->selectTool(InsertionTool); });
  ui->selectionToolButton->setCheckable(true);
  QObject::connect(ui->selectionToolButton, &QToolButton::clicked, this, [=](){ this->selectTool(SelectionTool); });
  // Modifiers
  QObject::connect(ui->normalModX, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->normalModY, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->normalModZ, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->modifierReset, &QPushButton::clicked, this, &EditWidget::resetNormalModifier);
  // OptiTrack settings
  QObject::connect(ui->optiTrackLength, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateOptiTrackSettings);
  QObject::connect(ui->optiTrackRadius, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateOptiTrackSettings);
  QObject::connect(ui->optiTrackLoadDefaults, &QPushButton::clicked, this, &EditWidget::resetOptiTrackSettings);
}

EditWidget::~EditWidget() {
  delete ui;
}

void EditWidget::updatePositions(osg::Vec3 point) {
  ui->anchorX->setText(QString::number(point.x()));
  ui->anchorY->setText(QString::number(point.y()));
  ui->anchorZ->setText(QString::number(point.z()));
}

void EditWidget::updateNormals(osg::Vec3 normal) {
  ui->normalX->setText(QString::number(normal.x()));
  ui->normalY->setText(QString::number(normal.y()));
  ui->normalZ->setText(QString::number(normal.z()));
}

void EditWidget::invalidatePositions() {
  ui->anchorX->setText("-");
  ui->anchorY->setText("-");
  ui->anchorZ->setText("-");
  ui->normalX->setText("-");
  ui->normalY->setText("-");
  ui->normalZ->setText("-");
}

void EditWidget::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  resetOptiTrackSettings();
}

void EditWidget::selectTool(Tool tool) {
  switch(tool) {
    case InsertionTool: {
      ui->insertionToolButton->setChecked(true);
      ui->selectionToolButton->setChecked(false);
      MainWindow::getInstance()->getOsgWidget()->getPicker()->setSelection(true);
      break;
    }
    case SelectionTool: {
      ui->insertionToolButton->setChecked(false);
      ui->selectionToolButton->setChecked(true);
      MainWindow::getInstance()->getOsgWidget()->getPicker()->setSelection(false);
      break;
    }
  }
}

void EditWidget::updateNormalModifier() {
  osg::Vec3 modifier = osg::Vec3(ui->normalModX->value(), ui->normalModY->value(), ui->normalModZ->value());
  MainWindow::getInstance()->getStore()->updateNormalModifier(modifier);
  MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
}

void EditWidget::resetNormalModifier() {
  osg::Vec3 modifier = osg::Vec3(0.0f, 0.0f, 0.0f);
  ui->normalModX->setValue(0.0f);
  ui->normalModY->setValue(0.0f);
  ui->normalModZ->setValue(0.0f);
  MainWindow::getInstance()->getStore()->updateNormalModifier(modifier);
  MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
}

void EditWidget::updateOptiTrackSettings() {
  OptiTrackSettings settings = {ui->optiTrackLength->value(), ui->optiTrackRadius->value()};
  MainWindow::getInstance()->getStore()->updateOptiTrackSettings(settings);
  MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
}

void EditWidget::resetOptiTrackSettings() {
  OptiTrackSettings settings = {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
  ui->optiTrackLength->setValue(OPTITRACK_DEFAULT_LENGTH);
  ui->optiTrackRadius->setValue(OPTITRACK_DEFAULT_RADIUS);
  MainWindow::getInstance()->getStore()->updateOptiTrackSettings(settings);
  MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
}
