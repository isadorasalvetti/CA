#ifndef RENDERMESH_H
#define RENDERMESH_H

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "collider.h"
#include "navmesh.h"

class RenderMesh {
public:
    bool init(QOpenGLShaderProgram *program, NavMesh &myNavMesh);

    //GL VAO n VBO
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* normBuffer;

    QMatrix4x4 modelMatrix;
    QVector3D color;

    void render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    void addPlanarColision(QVector<planeCollider> &vec);
    QVector3D correctPosition();

private:
    void addVertex(float v0, float v1, float v2);
    void addTriangle(int v0, int v1, int v2);
    void buildCube();
    void buildNormals();

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> triangles;
};

#endif // RENDERMESH_H
