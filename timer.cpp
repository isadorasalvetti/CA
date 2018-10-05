#include "timer.h"

Timer::Timer(QOpenGLWidget *s){
    widget = s;
    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(update()));
    t->start(30);
}

void Timer::update(){
   widget->update();
}
