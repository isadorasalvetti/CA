#include "particlespawner.h"
#include <math.h> 

const float particleRadius = 0.7f;
QVector<cilinderCollider> particleColliders;

const float baseSpeed = 0.025f;
const float modSpeed = 0.005f;

void particleSpawner::init(QOpenGLShaderProgram *prog, NavMesh &nm){
    //Delete old particles, if there are any
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    program = prog;
    myNavMesh = &nm;
    for (int i = 0; i<meshAmount; i++) {
        myMesh[i].init (program, SKL);
        speed[i] = baseSpeed + i*modSpeed;
    }
    if (!marker.init(prog)) cout << "Marker initialization failed" << endl;

    for (unsigned int i = 0; i < 20; i++){
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
        int m = particles[i]->meshId;
        if (i == 0){
            myMesh[m].color = QVector3D(1,0,0);
            myMesh[m].renderCharacter(gl, prog, modelMatrix);
            myMesh[m].color = QVector3D(1,1,1);
        } else myMesh[m].renderCharacter(gl, prog, modelMatrix);
    }
   renderMarkers(gl, prog);
}

void particleSpawner::renderMarkers(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    marker.color = QVector3D(0,1,1);
    marker.renderStatic(gl, prog, destinationMarker);

    marker.color = QVector3D(1,0,1);
    marker.renderStatic(gl, prog, originMarker);
}

void particleSpawner::genParticle(){
    iiPair gridPosition = myNavMesh->getRandomObjective().coords;
    QVector3D position = myNavMesh->gridToWorldPos(gridPosition);

    Particle *p = new Particle(position, program);

    p->positionInGrid = gridPosition;
    p->meshId = rand()%meshAmount;

    genParticleCollision();
    particles.push_back(p);
}

void particleSpawner::updateParticles(){
    for(int i = 0; i<particles.size(); i++){
        int s = particles[i]->meshId;
        if (particles[i]->updateNcheckObjective(speed[s])) //update position
            getNewPath(i); //updatePath
    }
    for(int i = 0; i<particles.size(); i++){ //Check colision
        //collsionCheck(particleColliders, i);
    }
    //Update animations
    for(int i = 0; i<meshAmount; i++) myMesh[i].updateCharacterAnimation(speed[i]);
}

void particleSpawner::getNewPath(int i){
    particles[i]->myPath = myNavMesh->getPathNObjective(particles[i]->currPosition);
    particles[i]->currPathCoord = 0;
    particles[i]->nextObjective = particles[i]->myPath[0];
    int s = particles[i]->myPath.size();
    updateMarkerMatrices(particles[i]->myPath[0], particles[i]->myPath[s]);
}

void particleSpawner::updateMarkerMatrices(QVector3D o, QVector3D d){
    destinationMarker.setToIdentity();
    destinationMarker.translate(d);
    originMarker.setToIdentity();
    originMarker.translate(o);
}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
