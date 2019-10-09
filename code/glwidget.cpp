#include <QFile>
#include <QString>
#include <QTransform>
#include <QtGui>
#include <QKeyEvent>
#include <iostream>
#include "glwidget.h"

using namespace std;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
     QWidget::setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

GLWidget::~GLWidget() {
}


void GLWidget::loadFaces(const QString &path) {
    string file_type = getFileExt(path);
    clear_variables();
    if(file_type == "json")
    {
        /*Implementation for json*/
        QFile json_file(path);
        if (!json_file.open(QIODevice::ReadOnly)) {
            qWarning("Failed to open file");
            exit(-1);
        }
        QByteArray json_data = json_file.readAll();
        QJsonDocument json_document(QJsonDocument::fromJson(json_data));
        face_collection.fromJson(json_document.array());

        face_collection.init=true;

    }

    else if(file_type == "stl")
    {
        QFile stl_file(path);
        if (!stl_file.open(QIODevice::ReadOnly)) {
            qWarning("Failed to open file");
            exit(-1);
        }
        face_collection.fromStl(path);

        face_collection.init=true;


    }

    else if(file_type == "pgm3d")
    {
        QFile pgm3D_file(path);
        if (!pgm3D_file.open(QIODevice::ReadOnly)) {
            qWarning("Failed to open file");
            exit(-1);
        }
        face_collection.frompgm3D(path);
        face_collection.init=true;
    }

    else if(file_type == "obj"){
        QFile obj_file(path);
        if (!obj_file.open(QIODevice::ReadOnly)) {
            qWarning("Failed to open file");
            exit(-1);
        }
        QByteArray obj_data = obj_file.readAll();
        face_collection.fromObj(path);
        face_collection.init=true;
    }

    else{
        std::cerr << "Invalid file type";
    }

    zoomScale = face_collection.init_scale;
    initializeGL();

}

string GLWidget::getFileExt(const QString& qs) {

   std::string s = qs.toLocal8Bit().constData();
   size_t i = s.rfind('.', s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return("");
}

void GLWidget::clear_variables(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    zoomScale = 1;
    first_paint = true;

}
void GLWidget::initializeGL() {
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f); // Set background color
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    setMouseTracking(true);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glDisable(GL_LIGHTING );
}

void GLWidget::paintGL() {
    QSize viewport_size = size();
    glViewport(0, 0, viewport_size.width(), viewport_size.height());
    float ar = (float) viewport_size.width() / (float) viewport_size.height();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    /*
   *  Config Projection Matrix  GL_PROJECTION
   */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                 // Reset the model-view matrix
    if(viewport_size.width() <= viewport_size.height()){
        glOrtho(-1.0, 1.0, -1 / ar, 1.0 / ar, 1.0, -1.0);
    }
    else{
        glOrtho(-1.0 *ar, 1.0 *ar, -1.0, 1.0, 1.0, -1.0);
    }

    /*
   *  Config Model View Matrix  GL_MODELVIEW
   */
    if (true == first_paint)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        first_paint = false;
    }
    else
    {
        glMatrixMode(GL_MODELVIEW);
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        glLoadIdentity();
        /*Rotation*/
        glRotatef(rotation_angle,rotation.y(),rotation.x(),0.0f);
        rotation_angle = 0;
        /*Translation*/
        glTranslatef(translation.x(),translation.y(),0.0f);
        translation.rx()=0;
        translation.ry()=0;
        /*Scale*/
        glScalef(zoomScale, zoomScale, zoomScale);
        zoomScale = 1;

        glMultMatrixf(m);
        glGetFloatv(GL_MODELVIEW_MATRIX, m);

    }


  //*********** Draw the model//
  // Define vertices in counter-clockwise (CCW) order with normal pointing out

  QVector3D view_coords;
  vector<pair<float, int>> vp;
  double max_value = 0;

  if(sorting== SORTING_ON)
  {
      for (int face_index=0;face_index<face_collection.faces.size();face_index++)
      {
          Face face = face_collection.faces[face_index];
          view_coords = object2view(face, m);
          vp.push_back(make_pair(view_coords.z(), face_index));
      }
      sort(vp.begin(), vp.end());
      reverse(vp.begin(),vp.end());

  }
  if(face_collection.init==true)
  {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBlendEquation(GL_FUNC_ADD);

      glBegin(face_collection.type);


      for (int face_index=0;face_index<face_collection.faces.size();face_index++)
      {
          Face face;
          if(sorting== SORTING_ON)
          {
               face = face_collection.faces[vp[face_index].second];
          }
          else
          {
               face = face_collection.faces[face_index];
          }

          float color = face.c;//face_index/(float)face_collection.faces.size() - 0.1;//face.c;//float)// //
          glColor4f(color, color, color, _alphaNew);
          for(int vertex_index = 0;vertex_index<face.vertices.size();vertex_index++)
          {
              glVertex3f( face.vertices[vertex_index].x(), face.vertices[vertex_index].y(), face.vertices[vertex_index].z());
              if(face.vertices[vertex_index].x()>max_value)
              {
                  max_value = face.vertices[vertex_index].x();
              }
          }
      }
       glEnd();
       glDisable(GL_BLEND);

  }
}


QVector3D GLWidget::object2view(Face face, GLfloat *model){

    QVector4D center, view_vec;
    QVector3D view_coords;



    center.setX((face.vertices[0].x() + face.vertices[2].x())/2.0);
    center.setY((face.vertices[0].y() + face.vertices[2].y())/2.0);
    center.setZ((face.vertices[0].z() + face.vertices[2].z())/2.0);
    center.setW(1.0);

    view_vec.setX(center.x()*model[0] + center.y()*model[4] + center.z()*model[8] + center.w()*model[12]);
    view_vec.setY(center.x()*model[1] + center.y()*model[5] + center.z()*model[9] + center.w()*model[13]);
    view_vec.setZ(center.x()*model[2] + center.y()*model[6] + center.z()*model[10] + center.w()*model[14]);
    view_vec.setW(center.x()*model[3] + center.y()*model[7] + center.z()*model[11] + center.w()*model[15]);

    view_coords.setX(view_vec.x()/view_vec.w());
    view_coords.setY(view_vec.y()/view_vec.w());
    view_coords.setZ(view_vec.z()/view_vec.w());

    return view_coords;

}

void GLWidget::getAlpha(int alpha){

    _alphaNew = 1 - ((float)alpha/100.0);
    paintGL();

}

void GLWidget::get_sorting_index(int index)
{
    sorting = index;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  // TODO:
  // - Capture mouse motions
  // - If left click is pressed: rotate
  // - If right click is pressed: translate
    //get mouse position
    QPoint displace(0,0);
    QPoint mouse_pos = event->pos();
    /*Code to check in what direction is the button preesed*/
    if( event->buttons() == Qt::RightButton ||  event->buttons() == Qt::LeftButton)
    {
        if((mouse_pos.x()-old_point_t.x()) > 0)
        {

            displace.setX(1);
            //translation.rx()+=0.01;
        }
        if((mouse_pos.x()-old_point_t.x()) < 0)
        {

            displace.setX(-1);
            //translation.rx()-=0.01;
        }

        if((mouse_pos.y()-old_point_t.y()) > 0)
        {
            displace.setY(-1);
            //translation.ry()-=0.01;
        }
        if((mouse_pos.y()-old_point_t.y()) < 0)
        {

            displace.setY(1);
            //translation.ry()+=0.01;
        }
    }
     /***************************************************/

    /* Left button = rotation*/
    if( event->buttons() == Qt::LeftButton)
    {
        rotation.rx()=-displace.x();
        rotation.ry()=displace.y();

        rotation_angle = speed_factor*2;
    }
    /* Right button = Translation*/
    if( event->buttons() == Qt::RightButton)
    {
        translation.rx()= (pow(speed_factor,1.5)*0.02*displace.x());
        translation.ry()= (pow(speed_factor,1.5)*0.02*displace.y());
    }

    /*Update points*/
    old_point_t.setX(mouse_pos.x());
    old_point_t.setY(mouse_pos.y());

    QWidget::update();

}

void GLWidget::wheelEvent(QWheelEvent *event)
{
  // TODO: zoom
    QPoint numDegrees = event->angleDelta();

    if (numDegrees.y() < 0)  zoomScale = (-1);
    if (numDegrees.y() > 0)  zoomScale = 1;
    zoomScale = pow((1.1*speed_factor),zoomScale);
    QWidget::update(); // call paintGL()
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() ==  Qt::Key::Key_Shift)
    {
        speed_factor = 2;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() ==  Qt::Key::Key_Shift)
    {
        speed_factor = 1;
    }
}
