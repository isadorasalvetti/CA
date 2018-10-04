#include "particlespawner.h"


void particleSpawner::init(int amount, QOpenGLShaderProgram *prog){
    QVector3D position = QVector3D(.5, 0, 0);
    float radius = 1;
    QVector3D color = QVector3D(1, 0, 0);
    QVector3D velocity = QVector3D(0, 0, 0);
    Particle *p = new Particle(position, radius, color, velocity, prog);
    particles.push_back(p);
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
}
