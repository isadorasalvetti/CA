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
    int lenght = 35;

    //Cloth simulation
    std::pair<int, int > size = std::pair<int, int> (6, 4);
    /* 01 02 03 04 05 06
     * 07 08 09 10 11 12
     * 13 14 15 16 17 18
     * 19 20 21 22 23 24
    */
    
    float spacing = 0.10f;
    
    static QVector<int> neightboors();

private:
    int count = 0;
    int dim = 0;

    QVector<planeCollider> planes;
    QVector<triangleCollider> tris;
    QVector<sphereCollider> spheres;
    QOpenGLShaderProgram *program;

};

#endif // PARTICLESPAWNER_H
