#include "particlespawner.h"
#include <math.h> 

const float particleRadius = 0.7f;
QVector<cilinderCollider> particleColliders;

void particleSpawner::init(QOpenGLShaderProgram *prog, NavMesh &nm){
    //Delete old particles, if there are any
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    program = prog;
    myNavMesh = &nm;
    for (int i = 0; i<meshAmount; i++) myMesh[i].init (program, CALLY);
    marker.init(prog);

    for (unsigned int i = 0; i < 1; i++){
        genParticle();
        getNewPath(i);
    }
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
        cilinderCollider c(particles[i]->currPosition, particleRadius);
        particles[i]->myCollision = c;
    }
}


void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
   for(int i = 0; i<particles.size(); i++){
        QMatrix4x4 modelMatrix;
        QVector3D moveDirection = particles[i]->forwardDirection;

        modelMatrix.translate(particles[i]->currPosition);

        float turnAngle = atan2(moveDirection.x(), moveDirection.z());
        modelMatrix.rotate(-turnAngle*57.3f, QVector3D(0, -1, 0));

        modelMatrix.rotate(-90, QVector3D(1, 0, 0));
        myMesh[0].renderCharacter(gl, prog, modelMatrix);
    }
}

void renderMarker(){

}

void particleSpawner::genParticle(){
    iiPair gridPosition = myNavMesh->getRandomObjective().coords;
    QVector3D position = myNavMesh->gridToWorldPos(gridPosition);

    float radius = .05f;
    QVector3D velocity = QVector3D(static_cast<float>(rand())/static_cast<float>(RAND_MAX), 0, static_cast<float>(rand())/static_cast<float>(RAND_MAX));
    Particle *p = new Particle(position, program);

    p->positionInGrid = gridPosition;

    genParticleCollision();
    particles.push_back(p);
}

void particleSpawner::updateParticles(){
    for(int i = 0; i<particles.size(); i++){
        if (particles[i]->updateNcheckObjective()) getNewPath(i);
    }
    for(int i = 0; i<particles.size(); i++){
        //collsionCheck(particleColliders, i);
    }
    //Update animations
    for(int i = 0; i<meshAmount; i++) myMesh[i].updateCharacterAnimation(0.03f);
}

void particleSpawner::getNewPath(int i){
    particles[i]->myPath = myNavMesh->getPathNObjective(particles[i]->currPosition);
    particles[i]->currPathCoord = 0;
    particles[i]->nextObjective = particles[i]->myPath[0];
}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
