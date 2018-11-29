#include "particlespawner.h"

const float max = 0.2;
const float particleRadius = 0.7f;

void particleSpawner::init(QOpenGLShaderProgram *prog){
    //Delete old particles, if there are any
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    program = prog;
    for (unsigned int i = 0; i< 30; i++){
        genParticle();
    }

    //timer.start();
}

void particleSpawner::updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss){
    // Colliders received by other static objecs
    planes = p;
    tris = ts;
    spheres = ss;
}

void particleSpawner::genParticleCollision() {

    /*
    Generates a 2D cilinder collision surroundeing the particle.
    This is not static, must be updated.
    */

    for(int i = 0; i<particles.size(); i++){
        particles[i]->myCollision = cilinderCollider(particles[i]->m_Position, particleRadius);
        particles[i]->particlesList = particles;
    }
}


void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
    //timer.start();
}

void particleSpawner::genParticle(){
    float x = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    float y = 0;
    float z = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    QVector3D position = QVector3D(x, y, z);
    float radius = .05f;
    QVector3D color = QVector3D(0, 0, 0);
    QVector3D velocity = QVector3D(static_cast<float>(rand())/static_cast<float>(RAND_MAX), 0, static_cast<float>(rand())/static_cast<float>(RAND_MAX));
    Particle *p = new Particle(position, radius, color, velocity, program);
    genParticleCollision();
    particles.push_back(p);
}

void particleSpawner::updateParticles(){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->mUpdate();
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->collsionCheck(particleColliders);
    }
}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
