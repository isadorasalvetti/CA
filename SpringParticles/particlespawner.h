#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include "collider.h"

class particleSpawner
{
public:
    QVector<Particle*> particles;
    void init(QOpenGLShaderProgram *prog, int dimension);
    void genParticle();
    void updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void updateParticles();

    bool solver; //true = euler, false= verlet

    //Rope Simulation
    int lenght = 10;

    //Cloth simulation
    std::pair<int, int > size = std::pair<int, int> (6, 4);
    float spacing = 0.05f;

private:
    int count = 0;
    int dim = 0;

    QVector<planeCollider> planes;
    QVector<triangleCollider> tris;
    QVector<sphereCollider> spheres;
    QOpenGLShaderProgram *program;

};

#endif // PARTICLESPAWNER_H
