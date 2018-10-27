#include "particle.h"
#include <QOpenGLFunctions>

// Animation
//****************************************************
const QVector3D G(0, -9.8f, 0);

bool Particle::mUpdate(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres, bool solver){
    float  elapsedTime = .03f;
    QVector3D lastPosition = m_Position;

    //lifespan -= elapsedTime;
    //if (lifespan < 0) return false;

    if (solver){
        m_Velocity += G*elapsedTime;
        m_Position += elapsedTime * m_Velocity;
    }
    else {
        if (!lp){
            m_Position += elapsedTime * m_Velocity + 0.5 * G*elapsedTime*elapsedTime;
            lp = true;
        } else {
            m_Velocity = (m_Position - m_LastPosition) /elapsedTime;
            m_Position += (m_Position - m_LastPosition) +G*elapsedTime*elapsedTime;
        }
    }
    m_LastPosition = lastPosition;

    //collision check
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
    VAO.destroy();
    VAO.create();
    if (!VAO.isCreated()) return false;
    VAO.bind();

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

    VAO.release();

    program->release();
    return true;
}

void Particle::Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program){
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_Position);

    VAO.bind();
    program->setUniformValue("color", m_Color);
    program->setUniformValue("factor", m_Radius);
    program->setUniformValue("model", modelMatrix);
    gl.glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    VAO.release();
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
