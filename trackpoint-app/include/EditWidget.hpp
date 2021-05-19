#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class EditWidget; }
QT_END_NAMESPACE

class EditWidget: public QWidget {
  Q_OBJECT

public:
  EditWidget(QWidget* parent = nullptr);
  ~EditWidget();

private:
  Ui::EditWidget* ui;
};
