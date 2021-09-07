// Include own headers
#include "EditWidget.hpp"
#include "../gui/ui_EditWidget.h"

// Include modules
#include "OSGWidget.hpp"
#include "PickHandler.hpp"
#include "TrackPointRenderer.hpp"
#include "OpenScadRenderer.hpp"
#include "MeshTools.hpp"

// Include dependencies
#include <sstream>
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
  QObject::connect(ui->anchorX, &QDoubleSpinBox::valueChanged, this, &EditWidget::changePositions);
  QObject::connect(ui->anchorY, &QDoubleSpinBox::valueChanged, this, &EditWidget::changePositions);
  QObject::connect(ui->anchorZ, &QDoubleSpinBox::valueChanged, this, &EditWidget::changePositions);
  QObject::connect(ui->normalModX, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->normalModY, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->normalModZ, &QDoubleSpinBox::valueChanged, this, &EditWidget::updateNormalModifier);
  QObject::connect(ui->rotateAroundNormal, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateNormalRotation(false); });
  QObject::connect(ui->compensation, &QCheckBox::stateChanged, this, [=](){ this->updateCompensation(false); });
  QObject::connect(ui->modifierReset, &QPushButton::clicked, this, &EditWidget::resetNormalModifier);
  // OptiTrack settings
  QObject::connect(ui->optiTrackLength, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateOptiTrackSettings(false); });
  QObject::connect(ui->optiTrackRadius, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateOptiTrackSettings(false); });
  QObject::connect(ui->optiTrackLoadDefaults, &QPushButton::clicked, this, [=](){ this->updateOptiTrackSettings(true); });
  QObject::connect(ui->optiTrackSanityCheck, &QPushButton::clicked, this, &EditWidget::manualOptiTrackSanityCheck);
  QObject::connect(ui->optiTrackEnableSanityCheck, &QCheckBox::stateChanged, this, &EditWidget::setOptiTrackSanityCheckStatus);
  // EMF Track settings
  QObject::connect(ui->emfTrackWidth, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateEMFTrackSettings(false); });
  QObject::connect(ui->emfTrackHeight, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateEMFTrackSettings(false); });
  QObject::connect(ui->emfTrackDepth, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateEMFTrackSettings(false); });
  QObject::connect(ui->emfTrackLoadDefaults, &QPushButton::clicked, this, [=](){ this->updateEMFTrackSettings(true); });
  // StramVRTrack settings
  QObject::connect(ui->steamVrTrackLength, &QDoubleSpinBox::valueChanged, this, [=](){ this->updateSteamVRTrackSettings(false); });
  QObject::connect(ui->steamVrTrackLoadDefaults, &QPushButton::clicked, this, [=](){ this->updateSteamVRTrackSettings(true); });
  QObject::connect(ui->steamVrTrackCollisionCheck, &QPushButton::clicked, this, &EditWidget::manualSteamVRTrackCollisionCheck);
  QObject::connect(ui->steamVrTrackEnableCollisionCheck, &QCheckBox::stateChanged, this, &EditWidget::setSteamVRTrackCollisionCheckStatus);
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
  ui->exportProgress->setVisible(false);
  ui->exportLabel->setVisible(false);
}

EditWidget::~EditWidget() {
  delete ui;
}

void EditWidget::updatePositions(osg::Vec3 point) {
  ui->anchorX->setValue(point.x());
  ui->anchorY->setValue(point.y());
  ui->anchorZ->setValue(point.z());
}

void EditWidget::updateNormals(osg::Vec3 normal) {
  ui->normalX->setText(QString::number(normal.x()));
  ui->normalY->setText(QString::number(normal.y()));
  ui->normalZ->setText(QString::number(normal.z()));
}

void EditWidget::invalidatePositions() {
  ui->deleteTrackPoint->setEnabled(false);
  ui->anchorX->setValue(0.0f);
  ui->anchorY->setValue(0.0f);
  ui->anchorZ->setValue(0.0f);
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
  TrackPoint* point;
  switch(ui->tabWidget->currentIndex()) {
    case 0: {
      OptiTrackPoint* optiPoint = MainWindow::getInstance()->getStore()->getOptiTrackPoints()[id];
      setOptiTrackSettings(optiPoint->getLength(), optiPoint->getRadius());
      point = static_cast<TrackPoint*>(optiPoint);
      break;
    }
    case 1: {
      EMFTrackPoint* emfPoint = MainWindow::getInstance()->getStore()->getEMFTrackPoints()[id];
      setEMFTrackSettings(emfPoint->getWidth(), emfPoint->getHeight(), emfPoint->getDepth());
      point = static_cast<TrackPoint*>(emfPoint);
      break;
    }
    case 2: {
      SteamVRTrackPoint* steamVrPoint = MainWindow::getInstance()->getStore()->getSteamVRTrackPoints()[id];
      setSteamVRTrackSettings(steamVrPoint->getLength());
      point = static_cast<TrackPoint*>(steamVrPoint);
      break;
    }
    default: {
      ActionPoint* actionPoint = MainWindow::getInstance()->getStore()->getActionPoints()[id];
      setActionPointSettings(actionPoint->getIdentifier());
      point = static_cast<TrackPoint*>(actionPoint);
      break;
    }
  }
  updatePositions(point->getTranslation());
  updateNormals(point->getNormal());
  setNormalModifier(point->getNormalModifier());
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

  count = MainWindow::getInstance()->getStore()->getCount(EMFTrack);
  {
    QString countString("TRACKPOINTS SET: ");
    countString.append(QString::number(count));
    ui->emfTrackCount->setText(countString);
  }

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
  selectedPoint = -1;
  resetNormalModifier();
  updateOptiTrackSettings(true);
  updateEMFTrackSettings(true);
  updateSteamVRTrackSettings(true);
  resetActionPointSettings();
}

void EditWidget::setExportAvailable(bool available) {
  ui->exportButton->setVisible(available);
  ui->exportProgress->setVisible(!available);
  ui->exportLabel->setVisible(!available);
}

void EditWidget::setExportStatus(int jobs, int done) {
  ui->exportProgress->setValue(done);
  std::stringstream text;
  text << "Export running: " << done << " of " << jobs << " finished.";
  ui->exportLabel->setText(QString::fromUtf8(text.str().c_str()));
}

bool EditWidget::getOptiTrackSanityCheckStatus() {
  return _optiTrackSanityCheck;
}

bool EditWidget::getSteamVRTrackCollisionCheckStatus() {
  return _steamVrTrackCollisionCheck;
}

void EditWidget::selectTool(Tool tool) {
  switch(tool) {
    case InsertionTool: {
      ui->insertionToolButton->setChecked(true);
      ui->selectionToolButton->setChecked(false);
      MainWindow::getInstance()->getOsgWidget()->getPicker()->setSelection(true);
      resetAllSettings();
      invalidatePositions();
      setPositionEditing(true);
      break;
    }
    case SelectionTool: {
      ui->insertionToolButton->setChecked(false);
      ui->selectionToolButton->setChecked(true);
      MainWindow::getInstance()->getOsgWidget()->getPicker()->setSelection(false);
      setPositionEditing(false);
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
    MainWindow::getInstance()->getStore()->projectModified();
    if (activeTrackingSystem == OptiTrack) {
      MeshTools::optiTrackSanityCheck(MainWindow::getInstance()->getStore()->getOptiTrackPoints(), false);
    } else if (activeTrackingSystem == SteamVRTrack) {
      MeshTools::steamVrTrackCollisionCheck(MainWindow::getInstance()->getStore()->getSteamVRTrackPoints(), false, MainWindow::getInstance()->getOsgWidget()->getVerifyGroup());
    }
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
    MainWindow::getInstance()->getStore()->projectModified();
    if (activeTrackingSystem == OptiTrack) {
      MeshTools::optiTrackSanityCheck(MainWindow::getInstance()->getStore()->getOptiTrackPoints(), false);
    } else if (activeTrackingSystem == SteamVRTrack) {
      MeshTools::steamVrTrackCollisionCheck(MainWindow::getInstance()->getStore()->getSteamVRTrackPoints(), false, MainWindow::getInstance()->getOsgWidget()->getVerifyGroup());
    }
  }
}

void EditWidget::setNormalModifier(osg::Vec3 normalModifier) {
  ui->normalModX->setValue(normalModifier.x());
  ui->normalModY->setValue(normalModifier.y());
  ui->normalModZ->setValue(normalModifier.z());
}

void EditWidget::updateNormalRotation(bool reset) {
  float normalRotation;
  if (reset) {
    normalRotation = 0.0f;
  } else {
    normalRotation = ui->rotateAroundNormal->value();
  }
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateNormalRotation(normalRotation);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
    MainWindow::getInstance()->getStore()->getTrackPointById(selectedPoint, activeTrackingSystem)->updateNormalRotation(normalRotation);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
    MainWindow::getInstance()->getStore()->projectModified();
    if (activeTrackingSystem == OptiTrack) {
      MeshTools::optiTrackSanityCheck(MainWindow::getInstance()->getStore()->getOptiTrackPoints(), false);
    } else if (activeTrackingSystem == SteamVRTrack) {
      MeshTools::steamVrTrackCollisionCheck(MainWindow::getInstance()->getStore()->getSteamVRTrackPoints(), false, MainWindow::getInstance()->getOsgWidget()->getVerifyGroup());
    }
  }
}

void EditWidget::setNormalRotation(float normalRotation) {
  ui->rotateAroundNormal->value();
}

void EditWidget::updateCompensation(bool reset) {
  bool compensation;
  if (reset) {
    compensation = true;
  } else {
    compensation = ui->compensation->checkState() == Qt::Checked ? true : false;
  }
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateCompensation(compensation);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
    MainWindow::getInstance()->getStore()->getTrackPointById(selectedPoint, activeTrackingSystem)->updateCompensation(compensation);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
    MainWindow::getInstance()->getStore()->projectModified();
  }
}

void EditWidget::setCompensation(bool compensation) {
  if (compensation) {
    ui->compensation->setCheckState(Qt::Checked);
  } else {
    ui->compensation->setCheckState(Qt::Unchecked);
  }
}

void EditWidget::updateOptiTrackSettings(bool reset) {
  OptiTrackSettings settings;
  if (reset) {
    settings = {OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS};
    setOptiTrackSettings(OPTITRACK_DEFAULT_LENGTH, OPTITRACK_DEFAULT_RADIUS);
  } else {
    settings = {ui->optiTrackLength->value(), ui->optiTrackRadius->value()};
  }
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateOptiTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getOptiTrackPoints()[selectedPoint]->updateOptiTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(OptiTrack);
    MainWindow::getInstance()->getStore()->projectModified();
    MeshTools::optiTrackSanityCheck(MainWindow::getInstance()->getStore()->getOptiTrackPoints(), false);
  }
}

void EditWidget::setOptiTrackSettings(double length, double radius) {
  ui->optiTrackLength->setValue(length);
  ui->optiTrackRadius->setValue(radius);
}

void EditWidget::updateEMFTrackSettings(bool reset) {
  EMFTrackSettings settings;
  if (reset) {
    settings = {EMFTRACK_DEFAULT_WIDTH, EMFTRACK_DEFAULT_HEIGHT, EMFTRACK_DEFAULT_DEPTH};
    setEMFTrackSettings(EMFTRACK_DEFAULT_WIDTH, EMFTRACK_DEFAULT_HEIGHT, EMFTRACK_DEFAULT_DEPTH);
  } else {
    settings = {ui->emfTrackWidth->value(), ui->emfTrackHeight->value(), ui->emfTrackDepth->value()};
  }
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateEMFTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getEMFTrackPoints()[selectedPoint]->updateEMFTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(EMFTrack);
    MainWindow::getInstance()->getStore()->projectModified();
  }
}

void EditWidget::setEMFTrackSettings(double width, double height, double depth) {
  ui->emfTrackWidth->setValue(width);
  ui->emfTrackHeight->setValue(height);
  ui->emfTrackDepth->setValue(depth);
}

void EditWidget::updateSteamVRTrackSettings(bool reset) {
  SteamVRTrackSettings settings;
  if (reset) {
    settings = {STEAMVR_DEFAULT_LENGTH};
    setSteamVRTrackSettings(STEAMVR_DEFAULT_LENGTH);
  } else {
    settings = {ui->steamVrTrackLength->value()};
  }
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateSteamVRTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPicker()->updateRenderer();
  } else {
    MainWindow::getInstance()->getStore()->getSteamVRTrackPoints()[selectedPoint]->updateSteamVRTrackSettings(settings);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(SteamVRTrack);
    MainWindow::getInstance()->getStore()->projectModified();
    MeshTools::steamVrTrackCollisionCheck(MainWindow::getInstance()->getStore()->getSteamVRTrackPoints(), false, MainWindow::getInstance()->getOsgWidget()->getVerifyGroup());
  }
}

void EditWidget::setSteamVRTrackSettings(double length) {
  ui->steamVrTrackLength->setValue(length);
}

void EditWidget::updateActionPointSettings(QString qinput) {
  std::string input = qinput.toUtf8().constData();
  if (MainWindow::getInstance()->getStore()->actionPointIdentifierInUse(input, selectedPoint) > 0) {
    ui->actionPointDoubleIdentifier->setVisible(true);
  } else {
    ui->actionPointDoubleIdentifier->setVisible(false);
    QString qtext = ui->actionPointIdentifier->text();
    ActionPointSettings settings = {qtext.toUtf8().constData()};
    if (selectedPoint < 0) {
      MainWindow::getInstance()->getStore()->updateActionPointSettings(settings);
    } else {
      MainWindow::getInstance()->getStore()->getActionPoints()[selectedPoint]->updateActionPointSettings(settings);
      MainWindow::getInstance()->getStore()->projectModified();
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
    unsigned int result = MainWindow::getInstance()->getStore()->actionPointIdentifierInUse(candidate, selectedPoint);
    if (result == 0) {
      settings = {candidate};
      break;
    }
    iterator++;
  }
  ui->actionPointIdentifier->setText(QString::fromStdString(candidate));
  if (selectedPoint < 0) {
    MainWindow::getInstance()->getStore()->updateActionPointSettings(settings);
  } else {
    MainWindow::getInstance()->getStore()->getActionPoints()[selectedPoint]->updateActionPointSettings(settings);
    MainWindow::getInstance()->getStore()->projectModified();
  }
}

void EditWidget::setActionPointSettings(std::string identifier) {
  ui->actionPointIdentifier->setText(QString::fromStdString(identifier));
  if (MainWindow::getInstance()->getStore()->actionPointIdentifierInUse(identifier, selectedPoint) > 0) {
    ui->actionPointDoubleIdentifier->setVisible(true);
  } else {
    ui->actionPointDoubleIdentifier->setVisible(false);
  }
}

void EditWidget::deleteCurrentTrackPoint() {
  ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
  MainWindow::getInstance()->getStore()->removeTrackPoint(selectedPoint, activeTrackingSystem);
  selectedPoint = -1;
  switch(activeTrackingSystem) {
    case OptiTrack: {
      updateOptiTrackSettings(true);
      break;
    }
    case EMFTrack: {
      updateEMFTrackSettings(true);
      break;
    }
    case SteamVRTrack: {
      updateSteamVRTrackSettings(true);
      break;
    }
    case ActionPoints: {
      resetActionPointSettings();
      break;
    }
  }
  resetNormalModifier();
  invalidatePositions();
  MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
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

void EditWidget::changePositions() {
  osg::Vec3 origin = osg::Vec3(ui->anchorX->value(), ui->anchorY->value(), ui->anchorZ->value());
  if (selectedPoint >= 0) {
    ActiveTrackingSystem activeTrackingSystem = getSelectedTrackingSystem();
    MainWindow::getInstance()->getStore()->getTrackPointById(selectedPoint, activeTrackingSystem)->updatePositions(origin);
    MainWindow::getInstance()->getOsgWidget()->getPointRenderer()->render(activeTrackingSystem);
    MainWindow::getInstance()->getStore()->projectModified();
  }
}

void EditWidget::setPositionEditing(bool mode) {
  ui->anchorX->setReadOnly(mode);
  ui->anchorY->setReadOnly(mode);
  ui->anchorZ->setReadOnly(mode);
}

void EditWidget::manualOptiTrackSanityCheck() {
  MeshTools::optiTrackSanityCheck(MainWindow::getInstance()->getStore()->getOptiTrackPoints(), true);
}

void EditWidget::setOptiTrackSanityCheckStatus() {
  _optiTrackSanityCheck = ui->compensation->checkState() == Qt::Checked ? true : false;
}

void EditWidget::manualSteamVRTrackCollisionCheck() {
  MeshTools::steamVrTrackCollisionCheck(MainWindow::getInstance()->getStore()->getSteamVRTrackPoints(), true, MainWindow::getInstance()->getOsgWidget()->getVerifyGroup());
}

void EditWidget::setSteamVRTrackCollisionCheckStatus() {
  _steamVrTrackCollisionCheck = ui->compensation->checkState() == Qt::Checked ? true : false;
}
