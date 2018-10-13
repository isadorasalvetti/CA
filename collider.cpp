#include "collider.h"

bool Collider::pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane){
    //test if a collision happened
    float p1Direction = plane.n[0]*p1[0] + plane.n[1]*p1[1] + plane.n[2]*p1[2] + plane.d;
    float p2Direction = plane.n[0]*p2[0] + plane.n[1]*p2[1] + plane.n[2]*p2[2] + plane.d;
    return p1Direction * p2Direction < 0;
}

bool Collider::pointTriCollision(QVector3D p1, QVector3D p2, triangleCollider tri){
    float p1Direction = tri.n[0]*p1[0] + tri.n[1]*p1[1] + tri.n[2]*p1[2] + tri.d;
    float p2Direction = tri.n[0]*p2[0] + tri.n[1]*p2[1] + tri.n[2]*p2[2] + tri.d;
    if (p1Direction * p2Direction < 0){
        float a1 = triangleArea(tri.p1, tri.p2, p2);
        float a2 = triangleArea(p2, tri.p2, tri.p3);
        float a3 = triangleArea(tri.p1, p2, tri.p3);
        float a = triangleArea(tri.p1, tri.p2, tri.p3);
        return a1 + a2 + a3 - a>=0;
    }
    return false;
}

bool Collider::pointSphereCollision(QVector3D p1, float radius){

}

std::pair<QVector3D, QVector3D> Collider::updateParticle(QVector3D p2, QVector3D v2, planeCollider plane){
    QVector3D pC = p2 - (1+plane.b)*(QVector3D::dotProduct(plane.n,p2)+plane.d)*plane.n;
    QVector3D vC = v2 - (1+plane.b)*(QVector3D::dotProduct(plane.n,v2))*plane.n;

    std::pair<QVector3D, QVector3D> update(pC, vC);
    return update;
}

std::pair<QVector3D, QVector3D> Collider::updateParticle(QVector3D p2, QVector3D v2, triangleCollider tri){
    QVector3D pC = p2 - (1+tri.b)*(QVector3D::dotProduct(tri.n,p2)+tri.d)*tri.n;
    QVector3D vC = v2 - (1+tri.b)*(QVector3D::dotProduct(tri.n,v2))*tri.n;

    std::pair<QVector3D, QVector3D> update(pC, vC);
    return update;
}

float Collider::triangleArea(QVector3D a, QVector3D b, QVector3D c){
    QVector3D ab = a - b;
    QVector3D ac = a - c;
    return QVector3D::dotProduct(ab, ac)/2.0f;
}
