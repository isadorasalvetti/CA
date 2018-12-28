#include "particle.h"
#include <QOpenGLFunctions>

// Movement
//****************************************************
const QVector3D G(0, 0, 0);

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

bool Particle::updateNcheckObjective(){
    float  elapsedTime = .03f;
    LastPosition = currPosition;

    Velocity = (nextObjective-currPosition).normalized() * speed;
    currPosition += elapsedTime * Velocity;

    const float error = (elapsedTime*speed)/2.0 + 1.2e-07f;
    if (lessQVec3D(currPosition, nextObjective, error) && greaterQVec3D(currPosition, nextObjective, error)){
        if (currPathCoord < myPath.size() - 1){//end of path not reached. Get next node.
            currPathCoord += 1;
            nextObjective = myPath[currPathCoord];
        } else return true; //End of path reached.
    }
    return false;
}

void Particle::collsionCheck(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres){
    /*COLLISION CHECKS START HERE */
    //planes
    for (int i = 0; i<planes.size(); i++){
        bool check = Collider::pointPlaneCollision(LastPosition, currPosition, planes[i]);
        if (check) {
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(currPosition, Velocity, planes[i]);
            currPosition = nD.first; Velocity = nD.second;
        }
    }
    //triangles
    for (int i = 0; i<triangles.size(); i++){
        bool check = Collider::pointTriCollision(LastPosition, currPosition, triangles[i]);
        if (check) {
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(currPosition, Velocity, triangles[i]);
            currPosition = nD.first; Velocity = nD.second;
        }
    }

    //sphere
    for (int i = 0; i<spheres.size(); i++){
        bool check = Collider::pointSphereCollision(currPosition, spheres[i]);
        if (check) {
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(LastPosition, Velocity, spheres[i]);
            currPosition = nD.first; Velocity = nD.second;
        }
    }
}

 /*COLLISION CHECKS END HERE */



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

void Particle::Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program, RenderMesh &myMesh){
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(currPosition);
    myMesh.renderCharacter(gl, program, modelMatrix);

    // set the model transformation

/* //Render sphere marker
    VAO->bind();
    program->setUniformValue("color", m_Color);
    program->setUniformValue("factor", m_Radius);
    program->setUniformValue("model", modelMatrix);
    gl.glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    VAO->release();
*/
}



//****************************************************

Particle::Particle(QVector3D position, QOpenGLShaderProgram *prog){
    currPosition = position;
    m_Radius = 0.05f;

    //if(!BuildPlane(prog)){
    //    std::cout << "Could not create particle" << std::endl;
    //};
}


/* DESTROY PARTICLE*/
Particle::~Particle(){
    VAO->destroy();
    coordBuffer->destroy();
    indexBuffer->destroy();
}
