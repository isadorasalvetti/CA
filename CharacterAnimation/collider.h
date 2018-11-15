#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include <QVector3D>

class planeCollider{
public:
    QVector3D n;
    float d;
    float b; //bounciness
    planeCollider(QVector3D N, float D, float B): n(N), d(D), b(B) {}
    planeCollider() {}
};

class Collider{
public:
    static bool pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane);
    static bool pointTriCollision(QVector3D p1, QVector3D p2, triangleCollider tri);
    static bool pointSphereCollision(QVector3D p1, sphereCollider sphere);
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, planeCollider plane);
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, triangleCollider tri);
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, sphereCollider tri);

    static float triangleArea(QVector3D a, QVector3D b, QVector3D c);
};

#endif // COLLIDER_H
