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

bool frstPerson = false;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), angleX(0.0f),
    translationX(0.0f), distance(2.0f)
{
    program = new QOpenGLShaderProgram();
    program_particle = new QOpenGLShaderProgram();
}

GLWidget::~GLWidget()
{
    if(program) delete program;
    if(program_particle) delete program_particle;
}


void GLWidget::initializeGL()
{
    QOpenGLFunctions::initializeOpenGLFunctions();

    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/basic.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/basic.frag");
    program->link();

    program_particle->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/particle.vert");
    program_particle->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/particle.frag");
    program_particle->link();

    if(!program->isLinked() || !program_particle->isLinked())
    {
            cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
            cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program_particle->log().toStdString();
            QApplication::quit();
    }

    //myMesh.init(program, myNavMesh);
    myNavMesh.setProgram(program);
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
    setAerialModelview();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //Rendering
    if (frstPerson) setFPModelView();
    mySpawner.renderParticles(*this, program);
    myNavMesh.renderMesh(*this);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
//    // Rotation
//    if(event->buttons() & Qt::LeftButton)
//    {
//        angleX += rotationFactor * (event->y() - lastMousePos.y());
//        angleX = max(-maxRotationCamera, min(angleX, maxRotationCamera));
//        translationX += translationFactor * (event->x() - lastMousePos.x());
//    }
//    // Zoom
//    if(event->buttons() & Qt::RightButton)
//    {
//        distance += 0.01f * (event->y() - lastMousePos.y());
//        distance = max(minDistanceCamera, min(distance, maxDistanceCamera));
//    }

//    lastMousePos = event->pos();

    makeCurrent();
    //setAerialModelview();
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

void GLWidget::setAerialModelview(){
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(0.0f, 0.0f, -10);
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

void GLWidget::setFPModelView(){
    QVector3D position = mySpawner.particles[0]->currPosition
            //- QVector3D(0, 0, -0.9f);
            - mySpawner.particles[0]->forwardDirection;
    QVector3D camHeight = QVector3D(0, 1.0f, 0);
    QVector3D orientation = mySpawner.particles[0]->forwardDirection;
    QMatrix4x4 viewMatrix;

    float turnAngle = atan2(orientation.x(), orientation.z());
    viewMatrix.rotate(180, QVector3D(0, 1, 0));
    viewMatrix.rotate(-turnAngle*57.3f, QVector3D(0, 1, 0));
    viewMatrix.translate(-(position+camHeight));

    program->bind();
    program->setUniformValue("view", viewMatrix);
    program->setUniformValue("normalMatrix", viewMatrix.normalMatrix());
    program->release();
}

//************************************
//Interface
//************************************

void GLWidget::setViewType(viewType t){
    if (t == FP) frstPerson = true;
    else if (t == AERIAL){
        frstPerson = false;
        setAerialModelview();
    }
}
