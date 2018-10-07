#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include <QElapsedTimer>

class particleSpawner
{
public:
    QVector<Particle*> particles;
    void init(int amount, QOpenGLShaderProgram *prog);
    void renderParticles(QOpenGLFunctions &gl,QOpenGLShaderProgram *prog, QVector<planeCollider> &ps);
private:
    QElapsedTimer timer;

};

#endif // PARTICLESPAWNER_H
