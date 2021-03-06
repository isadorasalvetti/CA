#ifndef RENDERMESH_H
#define RENDERMESH_H

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "animatedcharacter.h"
#include "collider.h"
#include "navmesh.h"

static const QMatrix4x4 identity = QMatrix4x4();

class RenderMesh {
public:
    bool init(QOpenGLShaderProgram *program);
    bool init(QOpenGLShaderProgram *program, NavMesh &myNavMesh);
    bool init(QOpenGLShaderProgram *program, Character type);

    bool genBuffers(QOpenGLShaderProgram *program);
    bool fillBuffers();
    bool fillBuffers(float (&meshVertices)[90000], int &vertCount,
            float (&meshNormals)[90000], int &normCount,
            CalIndex (&meshFaces)[150000], int &facesCount);

    void rewriteMesh(vector<float> vecs, vector<int> faces = vector<int>(1, -1));

    //GL VAO n VBO
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* normBuffer;

    QMatrix4x4 modelMatrix;
    QVector3D color;

    void renderStatic(QOpenGLFunctions &gl, QOpenGLShaderProgram *program, QMatrix4x4 model = identity);
    void renderCharacter(QOpenGLFunctions &gl, QOpenGLShaderProgram *program, QMatrix4x4 modelMatrix = identity);

    void updateCharacterAnimation(float dt);
    void changeCharacterAnimation(int state);
    void addPlanarColision(QVector<planeCollider> &vec);
    static void buildNormals(vector<float> vertices, vector<int> triangles, vector<float> &normals);
    static void buildNormals(float vertices[], int kVertices, unsigned int triangles[], int kFaces, vector<float> &normals);
    QVector3D correctPosition();

private:
    void addVertex(float v0, float v1, float v2);
    void addTriangle(int v0, int v1, int v2);
    void buildCube(float cubesize);

    AnimatedCharacter animChar;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> triangles;
};

#endif // RENDERMESH_H
