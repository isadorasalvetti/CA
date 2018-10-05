#include "collider.h"

void Collider::pointPlaneCollision(QVector3D p1, QVector3D p2, QVector3D v2, planeCollider plane, Particle &particle){
    //test if a collision happened
    float p1Direction = plane.n[0]*p1[0] + plane.n[1]*p1[1] + plane.n[2]*p1[2] + plane.d;
    float p2Direction = plane.n[0]*p2[0] + plane.n[1]*p2[1] + plane.n[2]*p2[2] + plane.d;
    bool collisionTest = p1Direction * p2Direction < 0;

    //adjust position and velocity
    if (collisionTest){
    QVector3D pC = p2 - 2*(QVector3D::dotProduct(plane.n,p2)+plane.d)*plane.n;
    QVector3D vC = v2 - 2*(QVector3D::dotProduct(plane.n,v2))*plane.n;
    particle.m_Position = pC;    particle.m_Velocity = vC;
    }
}

