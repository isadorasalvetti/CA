#include "particle.h"
#include <QOpenGLFunctions>

// Animation
//****************************************************
const QVector3D G(0, -9.8f, 0);

bool Particle::mUpdate(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres,
                       bool &solver, QVector<Particle*> &particles, int &i, int &dim){
    float  elapsedTime = .03f; //fixed timestep
    QVector3D lastPosition = m_Position;
    
    QVector3D totForce = QVector3D(0, 0, 0);
    QVector3D force = QVector3D(0, 0, 0);
    
    if (dim==2){
        float sprSize = 0.05; //initial distance between particle - spring size
        if (i+1 < particles.size()){
            QVector3D posDiff = particles[i+1]->m_Position - m_Position;
            float velDot = QVector3D::dotProduct(particles[i+1]->m_Velocity - m_Velocity, posDiff.normalized());
            p1Force = (kE*posDiff.length()-sprSize + kD*velDot) * posDiff.normalized();
            p2Force = - p1Force;
            if (i == 0){
                totForce = p1Force;
            } else {
                totForce = p1Force + particles[i-1]->p2Force;
            }
        }
    } else if (dim==3){
        QVector3D intForce = QVector3D(0, 0, 0);
        QVector3D rstForce = QVector3D(0, 0, 0);
        QVector3D extForce = QVector3D(0, 0, 0);
    }

    if (i!= 0) force = totForce+G;

        if (solver){
            m_Velocity += force*elapsedTime;
            m_Position += elapsedTime * m_Velocity;
        }
        else {
            if (!lp){
                m_Position += elapsedTime * m_Velocity + 0.5*force*elapsedTime*elapsedTime;
                lp = true;
            } else {
                m_Velocity = (m_Position - m_LastPosition) /elapsedTime;
                m_Position += kD*(m_Position - m_LastPosition) +force*elapsedTime*elapsedTime;
            }
        }
        m_LastPosition = lastPosition;

        fixPrticleSpacing(particles, dim, i);

    //COLLISION check
    //planes

    for (int i = 0; i<planes.size(); i++){
        bool check = Collider::pointPlaneCollision(m_LastPosition, m_Position, planes[i]);
        if (check) {
            lp = false;
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_Position, m_Velocity, planes[i]);
            m_Position = nD.first; m_Velocity = nD.second;
        }
    }
    if (true){
        //triangles
        for (int i = 0; i<triangles.size(); i++){
            bool check = Collider::pointTriCollision(m_LastPosition, m_Position, triangles[i]);
            if (check) {
                lp = false;
                std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_Position, m_Velocity, triangles[i]);
                m_Position = nD.first; m_Velocity = nD.second;
            }
        }

        //sphere
        for (int i = 0; i<spheres.size(); i++){
            bool check = Collider::pointSphereCollision(m_Position, spheres[i]);
            if (check) {
                lp = false;
                std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_LastPosition, m_Velocity, spheres[i]);
                m_Position = nD.first; m_Velocity = nD.second;
            }
        }
    }
    return true;
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

void Particle::Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program){
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_Position);

    VAO->bind();
    program->setUniformValue("color", m_Color);
    program->setUniformValue("factor", m_Radius);
    program->setUniformValue("model", modelMatrix);
    gl.glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    VAO->release();
}

//****************************************************

void Particle::fixPrticleSpacing(QVector<Particle*> &particles, int &dim, int &i){
    if (i > 0){
        QVector3D p2p1 = particles[i-1]->m_Position - m_Position;
        if (p2p1.length() > 0.05){ //if distance between particles is bigger than initila distance
            m_Position = particles[i-1]->m_Position - p2p1.normalized() * 0.10;
        }
    }
}

QVector<int> Particle::neighborhoodForFabric(int role, int i, std::pair<int, int> size){

}


//****************************************************

Particle::Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog){
    i_Position = position;

    m_Velocity = velocity;
    m_Position = position;
    m_Radius = radius;
    m_Color = color;

    if(!BuildPlane(prog)){
        std::cout << "Could not create particle" << std::endl;
    };
}

void Particle::DestroySelf(){
    VAO->destroy();
    coordBuffer->destroy();
    indexBuffer->destroy();
}
