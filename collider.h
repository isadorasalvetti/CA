#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include <QVector3D>
#include "particle.h"

class planeCollider{
public:
    QVector3D n;
    float d;
    planeCollider(QVector3D N, float D): n(N), d(D) {}

};

class Collider{
public:
    static void pointPlaneCollision
        (QVector3D p1, QVector3D p2, QVector3D v2, planeCollider plane, Particle &particle);
    static void pointSphereCollision();
};

#endif // COLLIDER_H
