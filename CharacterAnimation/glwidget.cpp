#include "glwidget.h"
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>

#include "rendermesh.h"
#include "particlespawner.h"


using namespace std;

const float rotationFactor = 0.5f;
const float translationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), angleX(0.0f), translationX(0.0f), distance(2.0f)
{
    program = nullptr;
    program_particle = nullptr;
}

GLWidget::~GLWidget()
{
    if(program) delete program;
    if(program_particle) delete program_particle;
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
            cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program_particle->log().toStdString();
            QApplication::quit();
    }

    myMesh.init(program, myNavMesh);
    mySpawner.init(program_particle, myNavMesh);
    timer = new Timer(this, &mySpawner);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //Default render flags.
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    program->bind();
    //Rendering
    glDisable(GL_CULL_FACE);
    myMesh.renderStatic(*this, program);
    glEnable(GL_CULL_FACE);
    //objectColliders.render(*this, program);
    program->release();

    // set the light position and attributes
    const GLfloat lightPosition[] = { 1.0f, -1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    const GLfloat lightColorAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightColorAmbient);
    const GLfloat lightColorDiffuse[] = { 0.52f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColorDiffuse);
    const GLfloat lightColorSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColorSpecular);

    // set the global OpenGL states
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // we will use vertex arrays, so enable them
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    mySpawner.renderParticles(*this, program_particle);

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
        translationX += translationFactor * (event->x() - lastMousePos.x());
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
    QMatrix4x4 viewMatrix;

    viewMatrix.translate(0.0f, 0.0f, -8);
    viewMatrix.rotate(45, 1.0f, 0.0f, 0.0f);
    viewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    //viewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);

    program_particle->bind();
    program_particle->setUniformValue("view", viewMatrix);
    program_particle->release();

    program->bind();
    program->setUniformValue("view", viewMatrix);
    program->setUniformValue("normalMatrix", viewMatrix.normalMatrix());
    program->release();

}

//************************************
//Interface
//************************************

void GLWidget::Reset(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
