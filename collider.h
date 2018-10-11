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

class triangleCollider{
public:
    QVector3D n;
    QVector3D tri;
    float d;
    float b; //bounciness
    triangleCollider(QVector3D N, QVector3D Tri, float D, float B): n(N), tri(Tri), d(D), b(B) {}
    triangleCollider() {}
};

class Collider{
public:
    static bool pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane);
    static bool pointTriCollision(QVector3D p1, QVector3D p2, triangleCollider tri);
    static void pointSphereCollision();
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, planeCollider plane);
};

#endif // COLLIDER_H
