#ifndef PLARTICLE_H
#define PLARTICLE_H

#include <QVector3D>
#include <iostream>
#include <fstream>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class Particle{
public:
    Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog);
    QVector3D  m_Position; // Center point of particle
    QVector3D  m_Velocity; // Current particle velocity
    QVector3D  m_Color;    // Particle color
    float m_Radius; //size of the particle

    void Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);

private:
    void mUpdate(float timeElapsed);
    bool BuildPlane(QOpenGLShaderProgram *program);

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;

};

#endif // PLARTICLE_H
