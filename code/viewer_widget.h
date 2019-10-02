#pragma once

#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>

#include "glwidget.h"

class ViewerWidget : public QWidget {
public:
  Q_OBJECT
public:
  ViewerWidget();

  QGridLayout *layout;
  QPushButton *load_file_button;
  GLWidget *gl_widget;

  QSlider *slider = new QSlider(Qt::Horizontal);

  QComboBox * Box = new QComboBox();
  QAction * firstAction;
  QAction * secondAction;

public slots:
  void alphaSlide(int);
  void loadFile();
  void boxCurrentIndexChanged(int);
};
