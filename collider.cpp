#include "collider.h"

bool Collider::pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane){
    //test if a collision happened
    float p1Direction = plane.n[0]*p1[0] + plane.n[1]*p1[1] + plane.n[2]*p1[2] + plane.d;
    float p2Direction = plane.n[0]*p2[0] + plane.n[1]*p2[1] + plane.n[2]*p2[2] + plane.d;
    return p1Direction * p2Direction < 0;
}

bool Collider::pointTriCollision(QVector3D p1, QVector3D p2, triangleCollider tri){
    //test if a collision happened
    float p1Direction = tri.n[0]*p1[0] + tri.n[1]*p1[1] + tri.n[2]*p1[2] + tri.d;
    float p2Direction = tri.n[0]*p2[0] + tri.n[1]*p2[1] + tri.n[2]*p2[2] + tri.d;
    return p1Direction * p2Direction < 0;
}

std::pair<QVector3D, QVector3D> Collider::updateParticle(QVector3D p2, QVector3D v2, planeCollider plane){
    QVector3D pC = p2 - (1+plane.b)*(QVector3D::dotProduct(plane.n,p2)+plane.d)*plane.n;
    QVector3D vC = v2 - (1+plane.b)*(QVector3D::dotProduct(plane.n,v2))*plane.n;

    std::pair<QVector3D, QVector3D> update(pC, vC);
    return update;
}
