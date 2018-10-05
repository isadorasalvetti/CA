#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>
#include <QOpenGLWidget>

class Timer: public QObject{
    Q_OBJECT

public:
    Timer(QOpenGLWidget *s);
private:
    QOpenGLWidget* widget;
public slots:
    void update();

};

#endif // TIMER_H
