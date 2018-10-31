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
#include "collider.h"

class Particle{
public:
    Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog);
    Particle(){}
    void DestroySelf();
    bool lp = false; //last position initialized
    QVector3D m_LastPosition;
    QVector3D i_Position;
    QVector3D m_Position; // Center point of particle
    QVector3D m_Velocity; // Current particle velocity
    QVector3D m_Color;    // Particle color
    float m_Radius; //size of the particle

    //Spring Vars
    const float kE = 10; //elasticity
    const float kD = 0.8f; //damping
    QVector3D p1Force;
    QVector3D p2Force;

    //Neighborhood

    void Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    bool mUpdate(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres,
                 bool &solver, QVector<Particle*> &particles, int &i, int &dim);

    void fixPrticleSpacing(QVector<Particle*> &particles, int &dim, int &i);

    QVector<int> neighborhoodForFabric(int role, int i, std::pair<int, int> size); //0 = streach, 1 = sheer, 2 = bend

private:
    bool BuildPlane(QOpenGLShaderProgram *program);

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
};

#endif // PLARTICLE_H
