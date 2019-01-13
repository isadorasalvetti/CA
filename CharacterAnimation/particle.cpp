#include "particle.h"
#include <QOpenGLFunctions>
#include "navmesh.h"

// Movement
//****************************************************
const QVector3D G(0, 0, 0);
float speed = 0.6f;
float maxDistanceSqr = 1.4f*1.4f;

// Collision
//****************************************************
float collRadius = 0.4f;

//Avoidance
//****************************************************
float coneRadius = 1.5f;
float coneAlpha = atan2(2*collRadius, coneRadius);
float cosineAlpha = cos(coneAlpha);

//Smooth turning
//****************************************************
float maxTurnAngleCos = 0.9;


bool lessQVec3D(const QVector3D &v1, const QVector3D &v2, const float &error){
    //v1 < v2
    return (v1.x() < v2.x() + error
         && v1.y() < v2.y() + error
         && v1.z() < v2.z() + error);
}

bool greaterQVec3D(const QVector3D &v1, const QVector3D &v2, const float &error){
    //v1 > v2
    return (v1.x() > v2.x() - error
         && v1.y() > v2.y() - error
         && v1.z() > v2.z() - error);
}

void Particle::updateDirection(){
    if (!stuckInWall){
        QVector3D lastForwardDirection = forwardDirection;
        nextforwardDirection = (nextObjective-currPosition).normalized();

        if (forwardDirection == QVector3D(0,0,0)) forwardDirection = nextforwardDirection;
        else if (forwardDirection != nextforwardDirection){
            QVector3D directionChange = nextforwardDirection+behaviorDirection;

            forwardDirection = forwardDirection*0.90f + directionChange*0.1f;
            forwardDirection.normalize();

            if (QVector3D::dotProduct(forwardDirection, nextforwardDirection) < 0.9f){
                if (QVector3D::dotProduct(forwardDirection, nextforwardDirection) < 0){
                    //SLOW MOST IF GOING BACK
                    forwardDirection = forwardDirection*0.8f + nextforwardDirection*0.2f;
                    speed = max(speed*0.75f, baseSpeed*0.1f);
                }
                //SLOW MOVEMENT IF TURNING TOO MUCH
                else speed = max(speed*0.85f, baseSpeed*0.6f);
            }
            else speed = speed*0.8f + baseSpeed*0.2f;
        }
        else speed = speed*0.8f + baseSpeed*0.2f;
    }
    else{
        //WALL COLLISION
        //stop and turn to the direction of the closest objective by a fixed amout.

        QVector3D ortogonalRef = QVector3D(nextforwardDirection.z(), 0, -nextforwardDirection.x());
        float dirAngle = QVector3D::dotProduct(ortogonalRef, collisionDirection);

        QMatrix4x4 rotation;
        if (dirAngle > 0) rotation.rotate(-5, QVector3D(0, 1, 0));
        else rotation.rotate(5, QVector3D(0, 1, 0));

        forwardDirection = rotation*forwardDirection;
        speed = speed*0.9f;
    }
}

int Particle::updatePosition(){
    LastPosition = currPosition;
    stuckInWall = (!verifyNextPosition());
    if (!stuckInWall) Velocity = forwardDirection * speed;
    else Velocity = QVector3D (0, 0, 0);
    currPosition += Velocity;

    if (myType == PATHFINDING) return checkObjective();
    return false;
}

int Particle::checkObjective(){
    //FOLLOWING PATH + SMOOTHNESS
    const float error = 0.3f;
    if (currPathCoord != 0 && (currPosition-nextObjective).lengthSquared() > maxDistanceSqr) return 2; //Chose a different path if too far from original path
    if (lessQVec3D(currPosition, nextObjective, error) && greaterQVec3D(currPosition, nextObjective, error)){
        if (currPathCoord < myPath.size() - 1){//Compute a differnt path if end is reached
            currPathCoord += 1;
            nextObjective = myPath[currPathCoord];
        } else return 1; //End of path reached.
    }
    else if (lessQVec3D(currPosition, myPath[currPathCoord+1], error) && greaterQVec3D(currPosition, nextObjective, error)){
        if (currPathCoord < myPath.size() - 2){//Compute a differnt path if end is reached
            currPathCoord += 2;
            nextObjective = myPath[currPathCoord];
        } else return 1; //End of path reached.
    }
    return 0; //Proceed normally
}

void Particle::addBehaviourForces(QVector<Particle *> &p, int me){
    if (!stuckInWall){
    behaviorDirection = QVector3D(0,0,0);

    //get future point
    QVector3D coneCenter = currPosition + forwardDirection*coneRadius;
    QVector3D halfConeSide = QVector3D(forwardDirection.z(), 0, -forwardDirection.x())*collRadius;

    //draw cone triangle
    if (me == 0){
        QVector3D pointA = currPosition + halfConeSide;
        QVector3D pointB = currPosition - halfConeSide;
        vector<float> cornerVertices;
        cornerVertices.push_back(coneCenter.x()); cornerVertices.push_back(coneCenter.y()); cornerVertices.push_back(coneCenter.z());
        cornerVertices.push_back(pointA.x()); cornerVertices.push_back(pointA.y()); cornerVertices.push_back(pointA.z());
        cornerVertices.push_back(pointB.x()); cornerVertices.push_back(pointB.y()); cornerVertices.push_back(pointB.z());
        cone.rewriteMesh(cornerVertices);
    }

    //OBSTACLE AVOIDANCE (other particles)
    for (int i = 0; i < p.size(); i++){
        if (i!=me){
            if(Collider::cilinderCilinderIntersection(coneCenter, p[i]->currPosition, coneRadius)){
                //get angle between particles
                QVector3D p2Cone = (coneCenter - p[i]->currPosition).normalized();
                float cos = QVector3D::dotProduct(p2Cone, forwardDirection);
                if (cos > 0 and cos > cosineAlpha){
                    QVector3D p1p2 = currPosition - p[i]->currPosition;
                    float avoidanceIntensity = 2*coneRadius - p1p2.length();
                    QVector3D avoidanceForce = QVector3D(p1p2.z(), 0, -p1p2.x()).normalized() * avoidanceIntensity;
                    behaviorDirection += avoidanceForce;
                }
            }
        }
    }

    if (behaviorDirection != QVector3D(0,0,0)) updateDirection();
    }
}

bool Particle::verifyNextPosition(){
    //is my next cell valid?
    iiPair gridPos = NavMesh::worldToGridPos(currPosition + (forwardDirection * speed));
    if (NavMesh::floorPlan[NavMesh::grid2index(gridPos)] == 0
     || NavMesh::floorPlan[NavMesh::grid2index(gridPos)] == 9){
        if (collisionDirection == QVector3D(0,0,0)) collisionDirection = forwardDirection;
        return true;
    }
    collisionDirection = QVector3D(0,0,0);
    return false;
}


// Plane
//****************************************************

bool Particle::BuildPlane(QOpenGLShaderProgram *program){
    GLfloat vertices[] = {-0.5f, -0.5f, 0, 0.5f, -0.5f, 0, 0.5f, 0.5f, 0, -0.5f, 0.5f, 0};
    GLuint faces[] = {0, 1, 3, 1, 2, 3};

    //adjust vertices by size/ position
    for (unsigned int i = 0; i < sizeof(vertices)/sizeof(GLfloat); i++){
        vertices[i] *= m_Radius;
    }

    program->bind();

    //My Buffers
    VAO = new QOpenGLVertexArrayObject;
    VAO->destroy();
    VAO->create();
    if (!VAO->isCreated()) return false;
    VAO->bind();

    coordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    coordBuffer->destroy();
    coordBuffer->create();
    if (!coordBuffer->isCreated()) return false;
    if (!coordBuffer->bind()) return false;
    coordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    coordBuffer->allocate(&vertices[0], sizeof(vertices));

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indexBuffer->destroy();
    indexBuffer->create();
    if (!indexBuffer->isCreated()) return false;
    if (!indexBuffer->bind()) return false;
    indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer->allocate(&faces[0], sizeof(faces));

    VAO->release();

    program->release();
    return true;
}


//****************************************************


Particle::Particle(QVector3D position, QOpenGLShaderProgram *prog){
    cone.init(prog);
    vector<int> faces;
    vector<float> v (9, 0);
    faces.push_back(0); faces.push_back(1); faces.push_back(2);
    cone.rewriteMesh(v, faces);
    currPosition = position;
    m_Radius = 0.05f;
}
