#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>
#include <QOpenGLWidget>
#include "particlespawner.h"

class Timer: public QObject{
    Q_OBJECT

public:
    Timer(QOpenGLWidget *s);
    particleSpawner spawner;
private:
    QOpenGLWidget* widget;
public slots:
    void update();

};

#endif // TIMER_H
