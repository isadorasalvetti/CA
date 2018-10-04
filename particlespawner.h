#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"

class particleSpawner
{
public:
    QVector<Particle*> particles;
    void init(int amount, QOpenGLShaderProgram *prog, GLWidget *gl);
    void renderParticles(QOpenGLFunctions &gl,QOpenGLShaderProgram *prog);

};

#endif // PARTICLESPAWNER_H
