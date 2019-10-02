#include "viewer_widget.h"
#include <QResizeEvent>
#include <QFileDialog>

ViewerWidget::ViewerWidget() {
  layout = new QGridLayout(this);
  load_file_button = new QPushButton("Load file");

  gl_widget = new GLWidget(this);
  //firstAction->setText("first action");
  layout->addWidget(load_file_button, 0, 0);
  connect(load_file_button, SIGNAL(released()), this, SLOT(loadFile()));
  layout->addWidget(gl_widget, 1, 0);
  layout->addWidget(slider);
  layout->addWidget(Box);
  Box->addItem("Sorting ON");
  Box->addItem("Sorting OFF");
  connect(Box, SIGNAL(currentIndexChanged(int)), this, SLOT(boxCurrentIndexChanged(int)));

  slider-> setRange(0,100);
  slider-> setTickInterval(1);
  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(alphaSlide(int)));
}

void ViewerWidget::loadFile() {  
  QString file_name= QFileDialog::getOpenFileName(this, ("Open File"),QDir::currentPath(), ("Document files (*.json *.stl *.obj)"));
  gl_widget->loadFaces(file_name);
  gl_widget-> update();
}

void ViewerWidget::alphaSlide(int alpha){
    gl_widget-> getAlpha(alpha);
    gl_widget-> update();
}

void ViewerWidget::boxCurrentIndexChanged(int index){
    gl_widget-> get_sorting_index(index);
    gl_widget-> update();
}
