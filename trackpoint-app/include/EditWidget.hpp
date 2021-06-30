#pragma once

// Include modules
#include "enums.hpp"

// Include dependencies
#include <QWidget>
#include <osg/Vec3>

QT_BEGIN_NAMESPACE
namespace Ui { class EditWidget; }
QT_END_NAMESPACE

class EditWidget: public QWidget {
  Q_OBJECT

public:
  EditWidget(QWidget* parent = nullptr);
  ~EditWidget();
  void updatePositions(osg::Vec3 point);
  void updateNormals(osg::Vec3 normal);
  void invalidatePositions();
  ActiveTrackingSystem getSelectedTrackingSystem();
  void setSelection(int id);
  int getSelectedPoint();
  void updateTrackpointCount();
  void resetActionPointSettings();
  void resetAllSettings();

protected:
  virtual void showEvent(QShowEvent* event);

private slots:
  void selectTool(Tool tool);
  void tabChanged(int index);
  void updateNormalModifier();
  void resetNormalModifier();
  void setNormalModifier(osg::Vec3 normalModifier);
  void updateNormalRotation(bool reset = false);
  void setNormalRotation(float normalRotation);
  void updateCompensation(bool reset = false);
  void setCompensation(bool compensation);
  void updateOptiTrackSettings(bool reset = false);
  void setOptiTrackSettings(double length, double radius);
  void updateEMFTrackSettings(bool reset = false);
  void setEMFTrackSettings(double width, double height, double depth);
  void updateSteamVRTrackSettings(bool reset = false);
  void setSteamVRTrackSettings(double length);
  void updateActionPointSettings(QString input);
  void setActionPointSettings(std::string identifier);
  void deleteCurrentTrackPoint();
  void exportProject();
  void changePositions();
  void setPositionEditing(bool mode);

private:
  Ui::EditWidget* ui;
  int selectedPoint = -1;
};
