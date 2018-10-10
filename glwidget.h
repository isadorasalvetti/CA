#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "mesh.h"
#include "particlespawner.h"
#include "collider.h"
#include "objects.h"

QT_BEGIN_NAMESPACE

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

QT_END_NAMESPACE

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    //Scene management
    QVector<planeCollider> planeColliders;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void setProjection(float aspect);
    void setModelview();
    //QMatrix4x4 projectionMatrix;
    //QMatrix4x4 modelviewMatrix;
    float angleX, angleY, distance;
    QPoint lastMousePos;

    QOpenGLShaderProgram *program;
    QOpenGLShaderProgram *program_particle;
    Mesh mesh;
    Objects objectColliders;
    particleSpawner spawner;
};

#endif
