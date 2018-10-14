#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include "collider.h"

class particleSpawner
{
public:
    QVector<Particle*> particles;
    void init(QOpenGLShaderProgram *prog);
    void genParticle();
    void updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void updateParticles();
private:
    int count = 0;

    //QElapsedTimer timer; ----! Replaced by a constant time step. Timer introduced too much error in solvers.
     QVector<planeCollider> planes;
     QVector<triangleCollider> tris;
     QVector<sphereCollider> spheres;
     QOpenGLShaderProgram *program;

};

#endif // PARTICLESPAWNER_H
