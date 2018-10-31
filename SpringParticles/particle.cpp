#include "particle.h"
#include <QOpenGLFunctions>

// Particle simulation
//****************************************************
const QVector3D G(0, -9.8f, 0);

bool Particle::mUpdate(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres,
                       bool &solver, QVector<Particle*> &particles, int &i, int &dim, std::pair<int, int> &size){
    float  elapsedTime = .03f; //fixed timestep
    QVector3D lastPosition = m_Position;
    
    //initial force = 0
    QVector3D totForce = QVector3D(0, 0, 0);
    QVector3D force = QVector3D(0, 0, 0);
    
    /* SPRING MESHES*/

    if (dim == 1){

        /* ROPE FORCE CALCULATION STARTS HERE */
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

            //Updates force for all particles except first.
            if (i!= 0) force = totForce+G;
        }


    } else if (dim == 2){
        /* CLOTH FORCE CALCULATION STARTS HERE */
        QVector3D intForce = QVector3D(0, 0, 0);
        //Internal forces
        QVector <int> Strech = neighborhoodForFabric(0, i, size);
        QVector <int> Shear = neighborhoodForFabric(1, i, size);
        QVector <int> Bend = neighborhoodForFabric(2, i, size);

        QVector3D rstForce = QVector3D(0, 0, 0);
        QVector3D extForce = QVector3D(0, 0, 0);
    }

        /* SOLVERS START HERE */
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

    /* RESTRICTION FIXING */
    if (dim == 1) fixPrticleSpacing(particles, dim, i);

    /*COLLISION CHECKS START HERE */
    //planes
    for (int i = 0; i<planes.size(); i++){
        bool check = Collider::pointPlaneCollision(m_LastPosition, m_Position, planes[i]);
        if (check) {
            lp = false;
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_Position, m_Velocity, planes[i]);
            m_Position = nD.first; m_Velocity = nD.second;
        }
    }
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
    return true;
}

/* CREATING THE PARTICLE/ BUFFERS */
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

QVector<int> Particle::neighborhoodForFabric(int role, int &i, std::pair<int, int> &size){
    QVector<int> neighbors;

    if (role == 0){ //this is Strech
        if(i - size.first > 0) neighbors.push_back(i - size.first); //up
        if(i + size.first < size.first*size.second) neighbors.push_back(i + size.first); //down
        if(i%size.first != 0) neighbors.push_back(i - 1); //left
        if((i+1)%size.first != 0) neighbors.push_back(i + 1); //right
    }

    else if (role == 1){ //this is Shear
        if((i)%size.first != 0){
            if(i-1-size.first > 0) neighbors.push_back(i - 1 - size.first); //up-left
            if(i-1+size.first < size.first*size.second) neighbors.push_back(i - 1 + size.first); //down-left

        }
        if((i+1)%size.first != 0){
            if(i+1-size.first > 0) neighbors.push_back(i + 1 - size.first); //up-right
            if(i+1+size.first < size.first*size.second) neighbors.push_back(i + 1 + size.first); //down-right
        }
    }

    else if (role == 2){ //this is Bend
        if(i - 2*size.first > 0) neighbors.push_back(i - 2*size.first); //up
        if(i + 2*size.first < size.first*size.second) neighbors.push_back(i + 2*size.first); //down
        if(i%size.first != 0 && (i-1)%size.first != 0) neighbors.push_back(i - 2); //left
        if((i+2)%size.first != 0 && (i+1)%size.first) neighbors.push_back(i + 2); //right
    }

    return neighbors;
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


/* DESTROY PARTICLE*/
Particle::~Particle(){
    VAO->destroy();
    coordBuffer->destroy();
    indexBuffer->destroy();
}
