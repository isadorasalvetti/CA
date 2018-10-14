#include "particlespawner.h"

const float max = 0.2;

void particleSpawner::init(QOpenGLShaderProgram *prog){
    program = prog;
    for (unsigned int i = 0; i< 25; i++){
        genParticle();
    }

    //timer.start();
}

void particleSpawner::updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss){
    planes = p;
    tris = ts;
    spheres = ss;
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
    //timer.start();
}

void particleSpawner::genParticle(){
    float x = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    float y = .7f;
    float z = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    QVector3D position = QVector3D(x, y, z);
    float radius = .05f;
    QVector3D color = QVector3D(static_cast<float>(rand())/static_cast<float>(RAND_MAX),
                                static_cast<float>(rand())/static_cast<float>(RAND_MAX),
                                static_cast<float>(rand())/static_cast<float>(RAND_MAX));
    QVector3D velocity = QVector3D(0, 0, 0);
    Particle *p = new Particle(position, radius, color, velocity, program);
    p->lifespan = 6 * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    particles.push_back(p);
}

void particleSpawner::updateParticles(){
    QVector<int> indicesToRemove;

    for(int i = 0; i<particles.size(); i++){
        if(!particles[i]->mUpdate(planes, tris, spheres)) indicesToRemove.append(i);
    }
    for(int i = 0; i<indicesToRemove.size(); i++){
//        QVector<Particle*>::iterator nth = particles.begin() + i;
//        particles.erase(nth);
        particles[i]->m_Position = particles[i]->i_Position;
        particles[i]->m_Velocity = QVector3D(0,0,0);
        particles[i]->lp = false;
        particles[i]->lifespan = 6 * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    }

    indicesToRemove.clear();

}
