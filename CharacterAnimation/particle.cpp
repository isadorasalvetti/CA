#include "particle.h"
#include <QOpenGLFunctions>

// Movement
//****************************************************
const QVector3D G(0, 0, 0);
float speed = 0.6f;

// Collision
//****************************************************
float collRadius = 0.5f;

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

bool Particle::updateNcheckObjective(float dt){
    LastPosition = currPosition;
    nextforwardDirection = (nextObjective-currPosition).normalized();

    if(repulsionDirection == QVector3D(0,0,0)){
        if (forwardDirection == QVector3D(0,0,0)) forwardDirection = nextforwardDirection;
        else if (forwardDirection != nextforwardDirection)
            forwardDirection = forwardDirection*0.85f + nextforwardDirection*0.15f;
    } else{
        repulsionDirection.normalize();
        QVector3D LastDirection = forwardDirection;
        forwardDirection = forwardDirection*0.50f + repulsionDirection*0.50f;
        if (QVector3D::dotProduct(forwardDirection, LastDirection)<0.3f)
            forwardDirection = QVector3D(0,0,0);
    }

    Velocity = forwardDirection * speed;
    currPosition += dt * Velocity;

    const float error = (dt*speed)/2.0 + 1.2e-07f;
    if (lessQVec3D(currPosition, nextObjective, error) && greaterQVec3D(currPosition, nextObjective, error)){
        if (currPathCoord < myPath.size() - 1){//end of path not reached. Get next node.
            currPathCoord += 1;
            nextObjective = myPath[currPathCoord];
        } else return true; //End of path reached.
    }
    return false;
}

void Particle::collisionCheck(QVector<Particle *> &p, int me){
    for (int i = me; i < p.size(); i++){
        if (i!=me){
            if(Collider::cilinderCilinderIntersection(currPosition, p[i]->currPosition, collRadius)){
                //There is a conflict between particle i and me.
                QVector3D rp = (p[i]->forwardDirection - forwardDirection).normalized() * (p[i]->currPosition - currPosition).length();
                repulsionDirection -= rp;
                p[i]->repulsionDirection += rp;
            }
        }
    }
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
    currPosition = position;
    m_Radius = 0.05f;
}
