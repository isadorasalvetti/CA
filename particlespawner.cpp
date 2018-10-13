#include "particlespawner.h"

const float max = 0.3;

void particleSpawner::init(int amount, QOpenGLShaderProgram *prog){
    for (int i =0; i < amount; i++){
        float x = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
        float y = .7f;
        float z = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);

        QVector3D position = QVector3D(x, y, z);
        float radius = .05f;
        QVector3D color = QVector3D(1, 0, 0);
        QVector3D velocity = QVector3D(0, 0, 0);
        Particle *p = new Particle(position, radius, color, velocity, prog);
        particles.push_back(p);
    }

    //timer.start();
}

void particleSpawner::updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts){
    planes = p;
    tris = ts;
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
    //timer.start();
}

void particleSpawner::updateParticles(){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->mUpdate(planes, tris);
    }
}
