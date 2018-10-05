#ifndef PLARTICLE_H
#define PLARTICLE_H

#include <QVector3D>
#include <iostream>
#include <fstream>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QTimer>

#include "glwidget.h"

class Particle{
public:
    Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog);
    QVector3D  m_Position; // Center point of particle
    QVector3D  m_Velocity; // Current particle velocity
    QVector3D  m_Color;    // Particle color
    float m_Radius; //size of the particle

    void Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);

public slots:
    void mUpdate();

private:
    bool BuildPlane(QOpenGLShaderProgram *program);


    QMatrix4x4 modelMatrix;

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
};

#endif // PLARTICLE_H
