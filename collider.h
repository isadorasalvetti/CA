#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include <QVector3D>

class planeCollider{
public:
    QVector3D n;
    float d;
    planeCollider(QVector3D N, float D): n(N), d(D) {}
    planeCollider() {}
};

class Collider{
public:
    static bool pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane);
    static void pointSphereCollision();
    static std::pair<QVector3D, QVector3D> updateParticle(QVector3D p2, QVector3D v2, planeCollider plane);
};

#endif // COLLIDER_H
