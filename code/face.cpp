#include "face.h"
#include <iostream>
#include <fstream>
#include "pgm3d.h"

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
    type = GL_QUADS;
    faces.clear();
    for (const QJsonValue &face : json) {
        Face new_face;
        new_face.fromJson(face.toObject());
        faces.push_back(new_face);
    }
    update_init_scale();
}

void FaceCollection::fromStl(const QString &path)
{
    type = GL_TRIANGLES;
    faces.clear();
    std::ifstream stlFile;
    stlFile.open(path.toStdString().c_str());
    if(!stlFile) // check if file can be found
    {
        std::cout<<"STL file not found."<<std::endl;
    }

    std::string ignore;
    stlFile >> ignore >> ignore; //ignore header: solid t=x.xx
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
        }
        stlFile >> ignore >> ignore; // endloop // endfacet
        faces.push_back(new_face);
    }
    update_init_scale();
}

void FaceCollection::frompgm3D(const QString &path) {
    std::ifstream pgmFile;
    pgmFile.open(path.toStdString().c_str());
    if(!pgmFile) // check if file can be found
    {
        std::cout<<"PGM-3D file not found."<<std::endl;
    }
    type = GL_QUADS;
    std::ifstream data(path.toStdString().c_str());

    string name;
    int line_counter = 0;
    int value=0;
    std::vector<int>voxels;
    pgm3D model3D;

    while(line_counter < 3){
        line_counter++;
        getline(data,name);
        if(line_counter == 2)
        {
            stringstream line_stream(name);
            line_stream >> model3D.x_rows >> model3D.y_cols >> model3D.z_layers;
        }
        if(line_counter == 3)
        {
            stringstream line_stream(name);
            line_stream >>model3D.max_color;
        }

        model3D.total_cubes = model3D.x_rows*model3D.y_cols*model3D.z_layers;
    }


  while(line_counter < model3D.total_cubes + HEADER_LINES)
  {
      line_counter++;
      getline(data,name);
      if(line_counter>HEADER_LINES)
      {
          stringstream line_stream(name);
          line_stream >> value;

          voxels.push_back(value);
          model3D.voxel_val.push_back(value);
      }
  }


  model3D.get_all_faces(faces);
  update_init_scale();

}


void FaceCollection::update_init_scale(void)
{
    init_scale = 1;

    float max_value = 0;
    for (int face_index=0;face_index<faces.size();face_index++)
    {
        for(int vertex_index = 0;vertex_index<faces[face_index].vertices.size();vertex_index++)
        {
            if(max_value < abs(faces[face_index].vertices[vertex_index].x()))
            {
                max_value = abs(faces[face_index].vertices[vertex_index].x());
            }
        }
    }
    init_scale = (1/max_value)*0.8f;
}
void FaceCollection::fromObj(const QString &path) {

    faces.clear();
    //    FILE * objFile = fopen(path.toStdString().c_str(), "r");
    std::ifstream in(path.toStdString().c_str(), std::ios::in);

    if(!in) // check if file can be found
    {
        //MessageBox(0, "OBJ file not found.", 0, 0);
        std::cout<<"OBJ file not found."<<std::endl;
    }

    vector<QVector3D> vertices_list;
    vector<QVector3D> normal_list;
    float x, y, z;
    std::string line;
    int i = 0;
    while (std::getline(in, line))
    {
        //check v for vertices
        if (line.substr(0,2)=="v "){
            std::istringstream v(line.substr(2));
            QVector3D Qvertex;
            v>>x;v>>y;v>>z;
            Qvertex.setX(x);
            Qvertex.setY(y);
            Qvertex.setZ(z);
            vertices_list.push_back(Qvertex);
        }
        //check vn for normals
        else if(line.substr(0,2)=="vn"){
            QVector3D normal;
            std::istringstream vn(line.substr(2));
            vn>>x;vn>>y;vn>>z;
            normal.setX(x);
            normal.setY(y);
            normal.setZ(z);
            normal_list.push_back(normal);

        }
        //check for faces
        else if(line.substr(0,2)=="f "){
            Face new_face;
            std::string delim = "/";
            std::string delim2 = " ";
            std::string line_seg = line.substr(1,std::string::npos);
            int vI, nI;
            std::vector<int> vertexIndex;
            std::vector<int> normalIndex;

            //sort through lines to retrieve vertices or normals, ignore texture
            size_t start = line_seg.find_last_of(' ', (line_seg.find_first_not_of(' ')));
            size_t end = line_seg.find(delim);
            size_t space = line_seg.find(delim2, start + delim2.length());
            int i = 0;
            if(end!= std::string::npos){
                while (end != std::string::npos)
                {
                    if (end < space){
                        if(i == 0){
                            std::istringstream fv(line_seg.substr(start, end -start));
                            fv>>vI;
                            vertexIndex.push_back(vI);
                        }

                        start = end + delim.length();
                        end = line_seg.find(delim, start);
                        i++;
                    }
                    else if(space < end){
                        std::istringstream fn(line_seg.substr(start, space -start));
                        fn>>nI;
                        normalIndex.push_back(nI);
                        i = 0;
                        start = space + line_seg.substr(space, end - space).find_last_of(' ', (line_seg.find_first_not_of(' ')));
                        std::istringstream fv(line_seg.substr(start, end - start));
                        fv>>vI;
                        vertexIndex.push_back(vI);
                        start = end + delim.length();
                        end = line_seg.find(delim, start);
                        space = line_seg.find(delim2, end);
                        i++;

                    }

                }
                if(i == 0){
                    std::istringstream fv(line_seg.substr(start, space -start));
                    fv>>vI;
                    vertexIndex.push_back(vI);
                }
                else if(i == 2){
                    std::istringstream fn(line_seg.substr(start, space -start));
                    fn>>nI;
                    normalIndex.push_back(nI);

                }
            }
            else{
                while(space != std::string::npos){
                    std::istringstream fv(line_seg.substr(start, space-start));
                    fv>>vI;
                    vertexIndex.push_back(vI);
                    start = space + line_seg.substr(space, std::string::npos).find_last_of(' ', (line_seg.substr(space, std::string::npos).find_first_not_of(' ')));
                    space = line_seg.find(delim2, start + delim2.length());
                    i++;
                }
                std::istringstream fv(line_seg.substr(start, space -start));
                fv>>vI;
                vertexIndex.push_back(vI);
            }

            //put vertices and normals into face
            int n_vertex = vertexIndex.size();
            if(n_vertex == 3){
                type = GL_TRIANGLES;
            }
            else if(n_vertex == 4){
                type = GL_QUADS;
            }
            else if(n_vertex > 4){
                qWarning("Obj may not render correctly,.obj reader only supports objects with faces up to order 4");
            }
            else{
                qWarning("Obj may not render correctly with this file format");
            }

            for (int n=0; n< n_vertex; n++){
                new_face.vertices.push_back(vertices_list[vertexIndex[n]-1]);
                if(!normalIndex.empty()){
                    new_face.normal.setX(normal_list[normalIndex[n]].x());
                    new_face.normal.setY(normal_list[normalIndex[n]].y());
                    new_face.normal.setZ(normal_list[normalIndex[n]].z());
                }

            }
            faces.push_back(new_face);
        }

    }
    update_init_scale();

}
