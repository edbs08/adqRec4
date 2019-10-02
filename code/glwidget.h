#pragma once

#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QString>
#include <QWidget>
#include <QtGui>
#include <string>

#include "face.h"

#define SHIFT_KEY 0x1000020

class GLWidget : public QOpenGLWidget {
public:
  Q_OBJECT
    static const int SORTING_ON = 0;
    static const int SORTING_OFF = 1;
    float _alphaNew = 1;
    int sorting = 0;

    float rotation_angle=0;

    float speed_factor = 1;
    QPointF translation=QPoint(0.0f,0.0f);
    QPointF rotation=QPoint(1.0f,0.0f);

    GLfloat m[16]={};
    QPoint old_point_t=QPoint(0.0f,0.0f);
public:
  GLWidget(QWidget *parent = 0);
  ~GLWidget();
  QSize sizeHint() const { return QSize(400, 400); }
  float zoomScale=1.0f;//0.0693433;
  QVector3D object2view(Face face, GLfloat *model);
  void loadFaces(const QString &path);
  void getAlpha(int);
  void get_sorting_index(int index);
  int counter = 0;
  bool first_paint = true;
  std::string getFileExt(const QString& s);

  void clear_variables(void);

protected:
  void initializeGL() override;
  void paintGL() override;

  void wheelEvent(QWheelEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

  FaceCollection face_collection;
};
