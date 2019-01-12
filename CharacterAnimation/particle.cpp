#include "particle.h"
#include <QOpenGLFunctions>
#include "navmesh.h"

// Movement
//****************************************************
const QVector3D G(0, 0, 0);
float speed = 0.6f;
float maxDistanceSqr = 1.3f*1.3f;

// Collision
//****************************************************
float collRadius = 0.4f;

//Avoidance
//****************************************************
float coneRadius = 1.5f;
float coneAlpha = atan2(2*collRadius, coneRadius);
float cosineAlpha = cos(coneAlpha);

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
    nextforwardDirection = (nextObjective-currPosition).normalized();
        if (forwardDirection == QVector3D(0,0,0)) forwardDirection = nextforwardDirection;
        else if (forwardDirection != nextforwardDirection){
            forwardDirection = forwardDirection*0.85f + (nextforwardDirection+behaviorDirection)*0.15f;
            forwardDirection.normalize();
        }
}

int Particle::updatePosition(float dt){
    LastPosition = currPosition;

    Velocity = forwardDirection * speed * computeCollisions(dt);
    currPosition += dt * Velocity;

    if (myType == PATHFINDING) return checkObjective(dt);
    return false;
}

int Particle::checkObjective(float dt){
    const float error = speed/2;
    if ((currPosition-nextObjective).lengthSquared() > maxDistanceSqr) return 2; //recalculate path from this position
    if (lessQVec3D(currPosition, nextObjective, error) && greaterQVec3D(currPosition, nextObjective, error)){
        if (currPathCoord < myPath.size() - 1){//end of path not reached. Get next node.
            currPathCoord += 1;
            nextObjective = myPath[currPathCoord];
        } else return 1; //End of path reached.
    }
    return 0;
}

void Particle::addBehaviourForces(QVector<Particle *> &p, int me){
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

bool Particle::computeCollisions(float dt){
    //is my next cell valid?
    iiPair gridPos = NavMesh::worldToGridPos(currPosition + (forwardDirection * speed * dt));
    if (NavMesh::floorPlan[NavMesh::grid2index(gridPos)] == 0
     || NavMesh::floorPlan[NavMesh::grid2index(gridPos)] == 9){
        return true;
    }
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
