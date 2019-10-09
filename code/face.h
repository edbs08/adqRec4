#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QVector3D>
#include "GL/gl.h"

QJsonArray vectorToJson(const QVector3D &vector);
QVector3D vectorFromJson(const QJsonArray &array);

class Face {
public:
  std::vector<QVector3D> vertices;
  QVector3D normal;
  float c;

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &json);
};

class FaceCollection {
public:
    FaceCollection();
    float init_scale = 1;
    std::vector<Face> faces;
    unsigned int type = GL_TRIANGLES; //default value is TRIANGLES
    bool init;
    void fromJson(const QJsonArray &json);
    void fromStl(const QString &path);
    void frompgm3D(const QString &path);
    void fromObj(const QString &path);

    void update_init_scale(void);
};
