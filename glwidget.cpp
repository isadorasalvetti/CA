#include "glwidget.h"
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>

#include "mesh.h"


using namespace std;


const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), angleX(0.0f), angleY(0.0f), distance(2.0f)
{
    program = NULL;
}

GLWidget::~GLWidget()
{
    if(program) delete program;
}


void GLWidget::initializeGL()
{
    QOpenGLFunctions::initializeOpenGLFunctions();

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/basic.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/basic.frag");
    program->link();
    if(!program->isLinked())
    {
            cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
            QApplication::quit();
    }

    if(!mesh.init(program))
    {
            cout << "Could not create vbo" << endl;
            QApplication::quit();
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //Default render flags.
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    cout << "OpenGL initialized" << endl;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    setProjection((float)w/h);
    setModelview();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->bind();
    //Rendering
    mesh.render(*this);
    program->release();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Rotation
    if(event->buttons() & Qt::LeftButton)
    {
        angleX += rotationFactor * (event->y() - lastMousePos.y());
        angleX = max(-maxRotationCamera, min(angleX, maxRotationCamera));
        angleY += rotationFactor * (event->x() - lastMousePos.x());
    }
    // Zoom
    if(event->buttons() & Qt::RightButton)
    {
        distance += 0.01f * (event->y() - lastMousePos.y());
        distance = max(minDistanceCamera, min(distance, maxDistanceCamera));
    }

    lastMousePos = event->pos();

    makeCurrent();
    setModelview();
    doneCurrent();
    update();
}

void GLWidget::setProjection(float aspect)
{
    QMatrix4x4 projectionMatrix;

    projectionMatrix.perspective(60, aspect, 0.01, 100.0);
    program->bind();
    program->setUniformValue("projection", projectionMatrix);
    program->release();
}

void GLWidget::setModelview()
{
    QMatrix4x4 modelviewMatrix;

    modelviewMatrix.translate(0, 0, -distance);
    modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);

    program->bind();
    program->setUniformValue("modelview", modelviewMatrix);
    program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    program->release();
}
