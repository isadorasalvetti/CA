#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include <QVector3D>

class sphereCollider{
public:
    QVector3D c;
    float r;
    float b; //bounciness
    sphereCollider(QVector3D C, float R, float B): c(C), r(R), b(B) {}
    sphereCollider() {}
};

class triangleCollider{
public:
    QVector3D n;
    QVector3D p1;
    QVector3D p2;
    QVector3D p3;
    float d;
    float b; //bounciness
    triangleCollider(QVector3D N, QVector3D P1, QVector3D P2, QVector3D P3, float D, float B):
        n(N), p1(P1), p2(P2), p3(P3), d(D), b(B) {}
    triangleCollider() {}
};

class planeCollider{
public:
    QVector3D n;
    float d;
    float b; //bounciness
    planeCollider(QVector3D N, float D, float B): n(N), d(D), b(B) {}
    planeCollider() {}
};

//2D only!
class cilinderCollider{
    public:
    QVector3D c;
    float r;
    cilinderCollider(QVector3D C, float R): c(C), r(R) {}
    cilinderCollider() {}
};

class Collider{
public:
    static bool pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane);
    static bool pointTriCollision(QVector3D p1, QVector3D p2, triangleCollider tri);
    static bool pointSphereCollision(QVector3D p1, sphereCollider sphere);
    static bool cilinderCilinderCollision(cilinderCollider c1, cilinderCollider c2);
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, planeCollider plane);
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, triangleCollider tri);
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, sphereCollider tri);
    static std::pair<QVector3D, QVector3D> updateParticleA(QVector3D p2, QVector3D v2, cilinderCollider cil);

    static float triangleArea(QVector3D a, QVector3D b, QVector3D c);
};

#endif // COLLIDER_H
