#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector3D>
#include <iostream>
#include <fstream>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QTimer>
#include "collider.h"

using namespace std;
typedef pair<int, int> iiPair;

class Particle{
public:
    Particle(QVector3D position, QOpenGLShaderProgram *prog);
    ~Particle();
    bool lp = false; //last position initialized
    QVector3D LastPosition;
    QVector3D currPosition; // Center point of particle
    QVector3D Velocity; // Current particle velocity
    QVector3D m_Color;    // Particle color
    cilinderCollider myCollision; //Collision information

    float m_Radius; //size of the particle

    iiPair positionInGrid;
    vector<QVector3D> myPath;
    int currPathCoord = 0;
    QVector3D nextObjective;

    void Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    void collsionCheck(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres); //Used for static objects (point based only!)
    void collsionCheck(QVector<cilinderCollider> &cilinders); //Used for collision with other particles (if cilinder boundary has been added)
    bool updateNcheckObjective(); //returns false when particle needs new objective

private:
    bool BuildPlane(QOpenGLShaderProgram *program);

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
};

#endif // PARTICLE_H
