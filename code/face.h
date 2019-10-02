#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QVector3D>

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


    std::vector<Face> faces;
    bool init;
    void fromJson(const QJsonArray &json);
    void fromStl(const QString &path);
};
