#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include "collider.h"

class particleSpawner
{
public:
    QVector<Particle*> particles;
    void init(int amount, QOpenGLShaderProgram *prog);
    void updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void updateParticles();
private:
    //QElapsedTimer timer; ----! Replaced by a constant time step. Timer introduced too much error in solvers.
     QVector<planeCollider> planes;
     QVector<triangleCollider> tris;


};

#endif // PARTICLESPAWNER_H
