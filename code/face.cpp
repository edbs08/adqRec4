#include "face.h"
#include <iostream>
#include <fstream>

QJsonArray vectorToJson(const QVector3D &vector) {
  QJsonArray result;
  for (int dim = 0; dim < 3; dim++)
    result.append(QJsonValue(vector[dim]));
  return result;
}

QVector3D vectorFromJson(const QJsonArray &array) {
  if (array.count() != 3) {
    throw std::runtime_error("Invalid size for vector: " +
                             std::to_string(array.count()));
  }
  QVector3D result;
  for (int dim = 0; dim < 3; dim++) {
    if (!array.at(dim).isDouble())
      throw std::runtime_error("Invalid value in vector at idx: " +
                               std::to_string(dim));
    result[dim] = array.at(dim).toDouble();
  }
  return result;
}

QJsonObject Face::toJson() const {
  QJsonArray vertices_json;
  for (const QVector3D &v : vertices)
    vertices_json.append(vectorToJson(v));
  QJsonObject result;
  result["vertices"] = vertices_json;
  result["normal"] = vectorToJson(normal);
  result["color"] = c;
  return result;
}

void Face::fromJson(const QJsonObject &json) {
  for (const std::string &field : {"vertices", "normal", "color"}) {
    if (!json.contains(QString(field.data())))
      throw std::runtime_error("Missing field '" + field + "' in json file");
  }
  vertices.clear();
  for (const QJsonValue &vertex : json["vertices"].toArray()) {
    vertices.push_back(vectorFromJson(vertex.toArray()));
  }
  normal = vectorFromJson(json["normal"].toArray());
  c = json["color"].toDouble();
}
FaceCollection::FaceCollection()
{
    init=false;
}
using namespace std;
void FaceCollection::fromJson(const QJsonArray &json) {
  faces.clear();
  for (const QJsonValue &face : json) {
    Face new_face;
    new_face.fromJson(face.toObject());
    faces.push_back(new_face);
  }
}

void FaceCollection::fromStl(const QString &path) {
    cout<<"from stl function"<<endl;
  faces.clear();
  std::ifstream stlFile;
  stlFile.open(path.toStdString().c_str());
  if(!stlFile) // check if file can be found
  {
      //MessageBox(0, "STL file not found.", 0, 0);
      std::cout<<"STL file not found."<<std::endl;
  }

  std::string ignore;
  stlFile >> ignore >> ignore; //ignore header: solid t=x.xx
  uint index = 0;

  int iIndex = 0;
  int vIndex = 0;
  //WORD indexTmp = 0;


  while(stlFile.peek() != EOF )
  {
      stlFile >> ignore >> ignore; // ignore "normal"
      if (ignore == "CUBE")
          break;

      float normal[3];
      Face new_face;
      stlFile >> normal[0] >> normal[1] >> normal[2]; // read and save the face normal
      new_face.normal.setX(normal[0]);
      new_face.normal.setY(normal[1]);
      new_face.normal.setY(normal[2]);
      stlFile >> ignore >> ignore; // ignore "outer loop"
      for(int i = 0; i <= 2; ++i) // read the three vertices of a face
      {

          float vertex[3]={};

          stlFile >> ignore >> vertex[0] >> vertex[1] >> vertex[2];// >> ignore >> ignore;
          QVector3D Qvertex;
          Qvertex.setX(vertex[0]);
          Qvertex.setY(vertex[1]);
          Qvertex.setZ(vertex[2]);
          new_face.vertices.push_back(Qvertex);
          //cout<<vertex[0]<<endl;
          //cout<<vertex[1]<<endl;
          //cout<<vertex[2]<<endl;
#if 0
          //if (!ContainsVertexIndex(vertexTmp, vertices, indexTmp)) // return vertex index of double
          //{
              mVertices.push_back(vertexTmp); //save newly created vertex

              indexTmp = vIndex; // create index reference to the new vertex
              vIndex++; // increment index
          //}
          mIndices.push_back(indexTmp);
          iIndex++; // increment index
#endif
      }
      stlFile >> ignore >> ignore; // endloop // endfacet
      faces.push_back(new_face);
  }

}