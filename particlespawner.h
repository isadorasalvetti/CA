#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"

class particleSpawner
{
public:
    QVector<Particle> particles;
    particleSpawner(int amount, QOpenGLShaderProgram *prog);
    void renderParticles(QOpenGLFunctions &gl);

};

#endif // PARTICLESPAWNER_H
