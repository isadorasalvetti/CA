#include "glwidget.h"
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>

#include "mesh.h"
#include "particlespawner.h"


using namespace std;

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), angleX(0.0f), angleY(0.0f), distance(2.0f)
{
    program = nullptr;
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

    program_particle = new QOpenGLShaderProgram();
    program_particle->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/particle.vert");
    program_particle->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/particle.frag");
    program_particle->link();

    if(!program->isLinked() || !program_particle->isLinked())
    {
            cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
            QApplication::quit();
    }

    if(!mesh.init(program))
    {
            cout << "Could not create vbo" << endl;
            QApplication::quit();
    }

    spawner.init(1, program_particle);

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
    //mesh.render(*this);
    program->release();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    program_particle->bind();
    spawner.renderParticles(*this, program_particle);
    program_particle->release();
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

    projectionMatrix.perspective(60, aspect, 0.01f, 100.0);
    program->bind();
    program->setUniformValue("projection", projectionMatrix);
    program->release();
    program_particle->bind();
    program_particle->setUniformValue("projection", projectionMatrix);
    program_particle->release();
}

void GLWidget::setModelview()
{
    QMatrix4x4 modelviewMatrix;

    modelviewMatrix.translate(0, 0, -distance);
    //Set particle model view with distance only.
    program_particle->bind();
    program_particle->setUniformValue("modelview", modelviewMatrix);
    program_particle->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    program_particle->release();

    modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
    //Set cube model view with rotation also.
    program->bind();
    program->setUniformValue("modelview", modelviewMatrix);
    program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    program->release();

}
