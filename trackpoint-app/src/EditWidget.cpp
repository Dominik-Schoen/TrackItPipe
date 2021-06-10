// Include own headers
#include "EditWidget.hpp"
#include "../gui/ui_EditWidget.h"

// Include modules
#include "OSGWidget.hpp"
#include "PickHandler.hpp"
#include "TrackPointRenderer.hpp"
#include "OpenScadRenderer.hpp"

// Include dependencies
#include <QFileDialog>

EditWidget::EditWidget(QWidget* parent): QWidget(parent), ui(new Ui::EditWidget) {
  ui->setupUi(this);
  ui->actionPointDoubleIdentifier->setVisible(false);
  ui->insertionToolButton->setCheckable(true);
  ui->insertionToolButton->setChecked(true);
  QObject::connect(ui->insertionToolButton, &QToolButton::clicked, this, [=](){ this->selectTool(InsertionTool); });
  ui->selectionToolButton->setCheckable(true);
  QObject::connect(ui->selectionToolButton, &QToolButton::clicked, this, [=](){ this->selectTool(SelectionTool); });
  QObject::connect(ui->tabWidget, &QTabWidget::currentChanged, this, &EditWidget::tabChanged);
  // Modifiers
  QObject::connect(ui->normalModX, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->normalModY, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->normalModZ, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->modifierReset, &QPushButton::clicked, this, &EditWidget::resetNormalModifier);
  // OptiTrack settings
  QObject::connect(ui->optiTrackLength, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateOptiTrackSettings);
  QObject::connect(ui->optiTrackRadius, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateOptiTrackSettings);
  QObject::connect(ui->optiTrackLoadDefaults, &QPushButton::clicked, this, &EditWidget::resetOptiTrackSettings);
  // StramVRTrack settings
  QObject::connect(ui->steamVrTrackLength, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateSteamVRTrackSettings);
  QObject::connect(ui->steamVrTrackLoadDefaults, &QPushButton::clicked, this, &EditWidget::resetSteamVRTrackSettings);
  // Action point settings
  QObject::connect(ui->actionPointIdentifier, &QLineEdit::textChanged, this, &EditWidget::updateActionPointSettings);
  QObject::connect(ui->actionPointLoadDefaults, &QPushButton::clicked, this, &EditWidget::resetActionPointSettings);
  // Delete button
  QObject::connect(ui->deleteTrackPoint, &QPushButton::clicked, this, &EditWidget::deleteCurrentTrackPoint);
  // Export button
  QObject::connect(ui->exportButton, &QPushButton::clicked, this, &EditWidget::exportProject);
  if (!OpenScadRenderer::openScadAvailable()) {
    ui->exportGroup->setEnabled(false);
  }
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
  ui->deleteTrackPoint->setEnabled(false);
  ui->anchorX->setText("-");
  ui->anchorY->setText("-");
  ui->anchorZ->setText("-");
  ui->normalX->setText("-");
  ui->normalY->setText("-");
  ui->normalZ->setText("-");
}

ActiveTrackingSystem EditWidget::getSelectedTrackingSystem() {
  switch(ui->tabWidget->currentIndex()) {
    case 0: {
      return OptiTrack;
    }
    case 1: {
      return EMFTrack;
    }
    case 2: {
      return SteamVRTrack;
    }
    default: {
      return ActionPoints;
    }
  }
}

void EditWidget::setSelection(int id) {
  selectedPoint = id;
  ui->deleteTrackPoint->setEnabled(true);
  switch(ui->tabWidget->currentIndex()) {
    case 0: {
      OptiTrackPoint* point = MainWindow::getInstance()->getStore()->getOptiTrackPoints()[id];
      updatePositions(point->getTranslation());
      updateNormals(point->getNormal());
      setNormalModifier(point->getNormalModifier());
      setOptiTrackSettings(point->getLength(), point->getRadius());
      break;
    }
    case 1: {
      break;
    }
    case 2: {
      SteamVRTrackPoint* point = MainWindow::getInstance()->getStore()->getSteamVRTrackPoints()[id];
      updatePositions(point->getTranslation());
      updateNormals(point->getNormal());
      setNormalModifier(point->getNormalModifier());
      setSteamVRTrackSettings(point->getLength());
      break;
    }
    default: {
      ActionPoint* point = MainWindow::getInstance()->getStore()->getActionPoints()[id];
      updatePositions(point->getTranslation());
      updateNormals(point->getNormal());
      setNormalModifier(point->getNormalModifier());
      setActionPointSettings(point->getIdentifier());
      break;
    }
  }
}

int EditWidget::getSelectedPoint() {
  return selectedPoint;
}

void EditWidget::updateTrackpointCount() {
  int count;
  count = MainWindow::getInstance()->getStore()->getCount(OptiTrack);
  {
    QString countString("TRACKPOINTS SET: ");
    countString.append(QString::number(count));
    ui->optiTrackCount->setText(countString);
  }

  // TODO: EMF Track

  count = MainWindow::getInstance()->getStore()->getCount(SteamVRTrack);
  {
    QString countString("TRACKPOINTS SET: ");
    countString.append(QString::number(count));
    ui->steamVrTrackCount->setText(countString);
  }

  count = MainWindow::getInstance()->getStore()->getCount(ActionPoints);
  {
    QString countString("ACTION POINTS SET: ");
    countString.append(QString::number(count));
    ui->actionPointCount->setText(countString);
  }
}

void EditWidget::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  resetAllSettings();
}

void EditWidget::resetAllSettings() {
  resetNormalModifier();
  resetOptiTrackSettings();
  resetSteamVRTrackSettings();
  resetActionPointSettings();
}

void EditWidget::selectTool(Tool tool) {
  switch(tool) {
    case InsertionTool: {
      ui->insertionToolButton->setChecked(true);
      ui->selectionToolButton->setChecked(false);
      MainWindow::getInstance()->getOsgWidget()->getPicker()->setSelection(true);
      invalidatePositions();
      resetAllSettings();
      selectedPoint = -1;
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

void EditWidget::tabChanged(int index) {
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  }
  selectedPoint = -1;
  ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
  MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
}

void EditWidget::updateNormalModifier() {
  osg::Vec3 modifier = osg::Vec3(ui->normalModX->value(), ui->normalModY->value(), ui->normalModZ->value());
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateNormalModifier(modifier);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
    MainWindow::getInstance()->getStore()->getTrackPointById(selectedPoint, activeTrackingSystem)->updateNormalModifier(modifier);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
  }
}

void EditWidget::resetNormalModifier() {
  osg::Vec3 modifier = osg::Vec3(0.0f, 0.0f, 0.0f);
  ui->normalModX->setValue(0.0f);
  ui->normalModY->setValue(0.0f);
  ui->normalModZ->setValue(0.0f);
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateNormalModifier(modifier);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
    MainWindow::getInstance()->getStore()->getTrackPointById(selectedPoint, activeTrackingSystem)->updateNormalModifier(modifier);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
  }
}

void EditWidget::setNormalModifier(osg::Vec3 normalModifier) {
  ui->normalModX->setValue(normalModifier.x());
  ui->normalModY->setValue(normalModifier.y());
  ui->normalModZ->setValue(normalModifier.z());
}

void EditWidget::updateOptiTrackSettings() {
  OptiTrackSettings settings = {ui->optiTrackLength->value(), ui->optiTrackRadius->value()};
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateOptiTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getOptiTrackPoints()[selectedPoint]->updateOptiTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(OptiTrack);
  }
}

void EditWidget::resetOptiTrackSettings() {
  OptiTrackSettings settings = {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
  ui->optiTrackLength->setValue(OPTITRACK_DEFAULT_LENGTH);
  ui->optiTrackRadius->setValue(OPTITRACK_DEFAULT_RADIUS);
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateOptiTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getOptiTrackPoints()[selectedPoint]->updateOptiTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(OptiTrack);
  }
}

void EditWidget::setOptiTrackSettings(double length, double radius) {
  ui->optiTrackLength->setValue(length);
  ui->optiTrackRadius->setValue(radius);
}

void EditWidget::updateSteamVRTrackSettings() {
  SteamVRTrackSettings settings = {ui->steamVrTrackLength->value()};
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateSteamVRTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getSteamVRTrackPoints()[selectedPoint]->updateSteamVRTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(SteamVRTrack);
  }
}

void EditWidget::resetSteamVRTrackSettings() {
  SteamVRTrackSettings settings = {STEAMVR_DEFAULT_LENGTH};
  ui->steamVrTrackLength->setValue(STEAMVR_DEFAULT_LENGTH);
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateSteamVRTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getSteamVRTrackPoints()[selectedPoint]->updateSteamVRTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(SteamVRTrack);
  }
}

void EditWidget::setSteamVRTrackSettings(double length) {
  ui->steamVrTrackLength->setValue(length);
}

void EditWidget::updateActionPointSettings(QString qinput) {
  std::string input = qinput.toUtf8().constData();
  if (MainWindow::getInstance()->getStore()->actionPointIdentifierInUse(input)) {
    ui->actionPointDoubleIdentifier->setVisible(true);
  } else {
    ui->actionPointDoubleIdentifier->setVisible(false);
    QString qtext = ui->actionPointIdentifier->text();
    ActionPointSettings settings = {qtext.toUtf8().constData()};
    if (selectedPoint < 0) {
      MainWindow::getInstance()->getStore()->updateActionPointSettings(settings);
    } else {
      MainWindow::getInstance()->getStore()->getActionPoints()[selectedPoint]->updateActionPointSettings(settings);
    }
  }
}

void EditWidget::resetActionPointSettings() {
  int iterator = 0;
  ActionPointSettings settings;
  std::string candidate;
  while (true) {
    candidate = "point";
    candidate += std::to_string(iterator);
    bool result = MainWindow::getInstance()->getStore()->actionPointIdentifierInUse(candidate);
    if (!result) {
      settings = {candidate};
      break;
    }
  }
  ui->actionPointIdentifier->setText(QString::fromStdString(candidate));
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateActionPointSettings(settings);
  } else {
    MainWindow::getInstance()->getStore()->getActionPoints()[selectedPoint]->updateActionPointSettings(settings);
  }
}

void EditWidget::setActionPointSettings(std::string identifier) {
  if (MainWindow::getInstance()->getStore()->actionPointIdentifierInUse(identifier)) {
    ui->actionPointDoubleIdentifier->setVisible(true);
  } else {
    ui->actionPointDoubleIdentifier->setVisible(false);
    ui->actionPointIdentifier->setText(QString::fromStdString(identifier));
  }
}

void EditWidget::deleteCurrentTrackPoint() {
  ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
  MainWindow::getInstance()->getStore()->removeTrackPoint(selectedPoint, activeTrackingSystem);
  selectedPoint = -1;
  switch(activeTrackingSystem) {
    case OptiTrack: {
      resetOptiTrackSettings();
      break;
    }
    case EMFTrack: {
      break;
    }
    case SteamVRTrack: {
      resetSteamVRTrackSettings();
      break;
    }
    case ActionPoints: {
      break;
    }
  }
  resetNormalModifier();
  invalidatePositions();
  MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(OptiTrack);
}

void EditWidget::exportProject() {
  Qt::CheckState optiTrackSelected = ui->optiTrackCkeckbox->checkState();
  Qt::CheckState emfTrackSelected = ui->emfTrackCheckbox->checkState();
  Qt::CheckState steamVrTrackSelected = ui->steamVrCheckbox->checkState();
  ExportSettings settings = {optiTrackSelected == Qt::Checked, emfTrackSelected == Qt::Checked, steamVrTrackSelected == Qt::Checked};
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export your TrackpointApp Project"), "", tr("3MF Files (*.3mf)"));
  std::string exportFile = fileName.toUtf8().constData();
  MainWindow::getInstance()->getStore()->exportProject(exportFile, settings);
}
