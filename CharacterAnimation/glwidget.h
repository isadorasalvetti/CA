﻿#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "navmesh.h"
#include "rendermesh.h"
#include "particlespawner.h"
#include "collider.h"
#include "timer.h"

QT_BEGIN_NAMESPACE

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

QT_END_NAMESPACE

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    //Scene management
    //colliders
    QVector<planeCollider> planeColliders;
    QVector<triangleCollider> triColliders;
    QVector<sphereCollider> sphereColliders;

    void Reset();

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
    float angleX, translationX, distance;
    QPoint lastMousePos;

    QOpenGLShaderProgram *program;
    QOpenGLShaderProgram *program_particle;
    Timer *timer;

    NavMesh myNavMesh;
    RenderMesh myMesh;
    particleSpawner mySpawner;

};

#endif
