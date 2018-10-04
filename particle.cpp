#pragma once
#include "particle.h"
#include <QOpenGLFunctions>

// Animation
//****************************************************
const QVector3D G(0, -9.8f, 0);

void Particle::mUpdate(float timeElapsed){
    m_Velocity += G;
    m_Position += m_Velocity * timeElapsed;
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
    VAO.bind();
    program->setUniformValue("color", m_Color);
    gl.glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    VAO.release();
}

//****************************************************

Particle::Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog){
    m_Velocity = velocity;
    m_Position = position;
    m_Radius = radius;
    m_Color = color;

    if(!BuildPlane(prog)){
        std::cout << "Could not create particle" << std::endl;
    };
}


/*
 *
void Plane::setPosition(const QVector3D& newPos){
    dconst = -glm::dot(newPos, normal);
};

bool Plane::isInside(const QVector3D& point){
    float dist;
    dist = glm::dot(point, normal) + dconst;
    if (dist > 1.e-7)
        return false;
    else
        return true;
};

float Plane::distPoint2Plane(const QVector3D& point){
    float dist;
    return dist = glm::dot(point, normal) + dconst;
};

QVector3D Plane::closestPointInPlane(const QVector3D& point){
    QVector3D closestP;
    float r = (-dconst - glm::dot(point, normal));
    return closestP = point + r*normal;
};

bool Plane::intersecSegment(const QVector3D& point1, const QVector3D& point2, QVector3D& pTall){
    if (distPoint2Plane(point1)*distPoint2Plane(point2) > 0)	return false;
    float r = (-dconst - glm::dot(point1, normal)) / glm::dot((point2 - point1), normal);
    pTall = (1 - r)*point1 + r*point2;
    return true;
};
*/
