#include "particlespawner.h"


particleSpawner::particleSpawner(int amount, QOpenGLShaderProgram *prog){
    QVector3D position = QVector3D(0, 0, 0);
    float radius = .2f;
    QVector3D color = QVector3D(1, 0, 0);
    QVector3D velocity = QVector3D(0, 0, 0);

    particles.push_back(Particle(position, radius, color, velocity, prog));
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl){
    for(int i = 0; i<particles.size(); i++){
        particles[i].Render(gl);
    }
}
