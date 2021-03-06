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
    void genParticle(int i);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void genParticleCollision();
    void updateParticles();
    void getNewPath(int i);
    void rewritePath(int i);

    bool solver; //true = euler, false= verlet
    void updateMarkerMatrices(vector<QVector3D> o);
    void renderMarkers(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);

private:
     int count = 0;
     NavMesh *myNavMesh;
     QVector<planeCollider> planes;
     QVector<triangleCollider> tris;
     QVector<sphereCollider> spheres;
     QOpenGLShaderProgram *program;
     static const int meshAmount = 15;
     RenderMesh myMesh[meshAmount];
     float speed[meshAmount];
     bool characterCreated = false;

     QMatrix4x4 destinationMarker;
     QMatrix4x4 originMarker;
     QVector<QMatrix4x4> pathMarkers;
     RenderMesh marker;

};

#endif // PARTICLESPAWNER_H
