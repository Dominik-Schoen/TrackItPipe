#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class NoMeshWidget; }
QT_END_NAMESPACE

class NoMeshWidget: public QWidget {
  Q_OBJECT

public:
  NoMeshWidget(QWidget* parent = nullptr);
  ~NoMeshWidget();

private slots:
  void loadMeshFile();
  void loadProjectFile();

private:
  Ui::NoMeshWidget* ui;
};
