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

protected:
  virtual void showEvent(QShowEvent* event);

private slots:
  void selectTool(Tool tool);
  void updateNormalModifier();
  void resetNormalModifier();
  void setNormalModifier(osg::Vec3 normalModifier);
  void updateOptiTrackSettings();
  void resetOptiTrackSettings();
  void setOptiTrackSettings(double length, double radius);
  void exportProject();

private:
  Ui::EditWidget* ui;
  int selectedPoint = -1;
};
