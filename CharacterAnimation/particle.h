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
#include "rendermesh.h"

using namespace std;
typedef pair<int, int> iiPair;

class Particle{
public:
    Particle(QVector3D position, QOpenGLShaderProgram *prog);
    ~Particle();
    bool characterGenerated = false;

    QVector3D currPosition; // Center point of particle
    QVector3D LastPosition;

    QVector3D Velocity; // Current particle velocity
    QVector3D m_Color;    // Particle color

    QVector3D forwardDirection = QVector3D(0,0,0);
    QVector3D nextforwardDirection;

    int meshId;

    cilinderCollider myCollision; //Collision information
    void setFPModelView(QOpenGLShaderProgram *prog);

    float m_Radius; //size of the particle

    vector<QVector3D> myPath;
    int currPathCoord = 0;
    QVector3D nextObjective;

    void collsionCheck(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres); //Used for static objects (point based only!)
    void collsionCheck(QVector<cilinderCollider> &cilinders); //Used for collision with other particles (if cilinder boundary has been added)
    bool updateNcheckObjective(float dt); //returns false when particle needs new objective

private:
    bool BuildPlane(QOpenGLShaderProgram *program);

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
};

#endif // PARTICLE_H
