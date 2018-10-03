#ifndef PLARTICLE_H
#define PLARTICLE_H

#include <QVector3D>

struct Particle
{
    Particle(): m_Position(0), m_Velocity(0), m_Color(0){
    }

    QVector3D   m_Position; // Center point of particle
    QVector4D   m_Velocity; // Current particle velocity
    QVector4D   m_Color;    // Particle color
};

typedef std::vector<Particle> ParticleBuffer;

#endif // PLARTICLE_H
