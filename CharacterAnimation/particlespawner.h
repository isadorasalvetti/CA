#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include "collider.h"
#include "navmesh.h"
#include "rendermesh.h"

class particleSpawner{
public:
    QVector<Particle*> particles;
    ~particleSpawner();
    void init(QOpenGLShaderProgram *prog, NavMesh &nm);
    void genParticle();
    void updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void genParticleCollision();
    void updateParticles();
    void getNewPath(int i);

    bool solver; //true = euler, false= verlet
    void updateMarkerMatrices(QVector3D o, QVector3D d);
    void renderMarkers(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);

private:
     int count = 0;
     NavMesh *myNavMesh;
     QVector<planeCollider> planes;
     QVector<triangleCollider> tris;
     QVector<sphereCollider> spheres;
     QOpenGLShaderProgram *program;
     static const int meshAmount = 4;
     RenderMesh myMesh[meshAmount];
     float speed[meshAmount];
     bool characterCreated = false;

     QMatrix4x4 destinationMarker;
     QMatrix4x4 originMarker;
     QVector<QMatrix4x4> pathMarkers;
     RenderMesh marker;

};

#endif // PARTICLESPAWNER_H
