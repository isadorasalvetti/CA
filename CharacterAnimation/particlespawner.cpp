#include "particlespawner.h"
#include <math.h> 

const float baseSpeed = 0.022f;
const float modSpeed = 0.005f;

void particleSpawner::init(QOpenGLShaderProgram *prog, NavMesh &nm){
    //Delete old particles, if there are any
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    program = prog;
    myNavMesh = &nm;
    for (unsigned int i = 0; i<meshAmount; i++){
        genParticle(i);
        getNewPath(i);
    }
    if (!marker.init(prog)) cout << "Marker initialization failed" << endl;
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        //UPDATE MODEL ROTATION
        QMatrix4x4 modelMatrix;
        QVector3D moveDirection = particles[i]->forwardDirection;

        modelMatrix.translate(particles[i]->currPosition);

        float turnAngle = atan2(moveDirection.x(), moveDirection.z());
        modelMatrix.rotate(-turnAngle*57.3f, QVector3D(0, -1, 0));
        modelMatrix.rotate(-90, QVector3D(1, 0, 0));

        if (particles[i]->speed < 0.003f)
            myMesh[i].changeCharacterAnimation(IDLE);
        else myMesh[i].changeCharacterAnimation(MOTION);


        if (i == 0){//FOR FIRST AGENT
            //RENDER CONE
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            particles[i]->cone.renderStatic(gl, prog);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            //CHANGE COLOR
            myMesh[i].color = QVector3D(1,0,1);
            myMesh[i].renderCharacter(gl, prog, modelMatrix);
            myMesh[i].color = QVector3D(1,1,1);
        } else myMesh[i].renderCharacter(gl, prog, modelMatrix);

        //        int gridPos = myNavMesh->grid2index(myNavMesh->worldToGridPos(particles[i]->currPosition));
        //        if (myNavMesh->floorPlan[gridPos]==0
        //         || myNavMesh->floorPlan[gridPos]==9) myMesh[m].color = QVector3D(1,1,1);
        //        else myMesh[m].color = QVector3D(1,1,0);

        iiPair gridPos = iiPair(1, 4);
        QVector3D worldPos = myNavMesh->gridToWorldPos(gridPos);
        iiPair testGridPos = myNavMesh->worldToGridPos(worldPos);
    }

    renderMarkers(gl, prog);
}

void particleSpawner::renderMarkers(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){    
    marker.color = QVector3D(0,1,1);
    marker.renderStatic(gl, prog, destinationMarker);

    for (int i = 0; i < pathMarkers.size(); i++){
        marker.color = QVector3D(1,1,0);
        marker.renderStatic(gl, prog, pathMarkers[i]);
    }
}

void particleSpawner::genParticle(int i){
    iiPair gridPosition = myNavMesh->getRandomObjective().coords;
    QVector3D position = myNavMesh->gridToWorldPos(gridPosition);

    if (i == 0) destinationMarker.translate(position);

    Particle *p = new Particle(position, program);
    p->myPath.push_back(position);
    particles.push_back(p);
    myMesh[i].init (program, SKL);
    particles[i]->baseSpeed = baseSpeed + (rand()/ static_cast <float>(RAND_MAX))*modSpeed;
}

void particleSpawner::updateParticles(){

    for(int i = 0; i<particles.size(); i++){
        particles[i]->updateDirection();
    }

    for(int i = 0; i<particles.size(); i++){
        particles[i]->addBehaviourForces(particles, i);
    }

    for(int i = 0; i<particles.size(); i++){
        int r = particles[i]->updatePosition();
        if (r==1)
            getNewPath(i); //updatePath
        else if (r==2) rewritePath(i);
    }

    //Update animations
    for(int i = 0; i<meshAmount; i++) myMesh[i].updateCharacterAnimation(particles[i]->speed);
}

void particleSpawner::getNewPath(int i){
    QVector3D lastDestination;
    lastDestination = particles[i]->myPath[particles[i]->myPath.size()-1];
    particles[i]->myPath = myNavMesh->getPathNObjective(lastDestination);
    particles[i]->currPathCoord = 0;
    particles[i]->nextObjective = particles[i]->myPath[0];
    if (i == 0) {
        int s = particles[i]->myPath.size();
        vector<QVector3D> pathCoords;
        for (int j = 0; j < s; j++) pathCoords.push_back(particles[i]->myPath[j]);
        updateMarkerMatrices(pathCoords);
    }
}

void particleSpawner::rewritePath(int i){
    //problem if it starts on a wall
    QVector3D lastDestination;
    lastDestination = particles[i]->currPosition;
    iiPair gridld = NavMesh::worldToGridPos(lastDestination);

    //get closest valid grid position if this one is not
    if (!NavMesh::isInGrid(gridld)
        || (NavMesh::floorPlan[NavMesh::grid2index(gridld)] !=0
        && NavMesh::floorPlan[NavMesh::grid2index(gridld)] !=9))
        for (int j=-1; j<=1; j++) for (int k=-1; k<=1; k++) if (j != k){
            QVector3D test = lastDestination + QVector3D (j, 0, k)*(NavMesh::scl/2);
            gridld = NavMesh::worldToGridPos(test);
            if (NavMesh::isInGrid(gridld)
                && (NavMesh::floorPlan[NavMesh::grid2index(gridld)] == 0
                || NavMesh::floorPlan[NavMesh::grid2index(gridld)] == 9)){
                lastDestination = test;
                break;
            }
        }

    particles[i]->myPath = myNavMesh->getPathNObjective(lastDestination);
    particles[i]->currPathCoord = 0;
    particles[i]->nextObjective = particles[i]->myPath[0];
    if (i == 0) {
        int s = particles[i]->myPath.size();
        vector<QVector3D> pathCoords;
        for (int j = 0; j < s; j++) pathCoords.push_back(particles[i]->myPath[j]);
        updateMarkerMatrices(pathCoords);
    }
}

void particleSpawner::updateMarkerMatrices(vector<QVector3D> o){
    pathMarkers.resize(o.size()-1);
    for (int i = 0; i < o.size()-1; i++){
        pathMarkers[i].setToIdentity();
        pathMarkers[i].translate(o[i]);
        pathMarkers[i].scale(QVector3D(0.5f, 0.5f, 0.5f));
    }
    destinationMarker.setToIdentity();
    destinationMarker.translate(o[o.size()-1]);
}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
