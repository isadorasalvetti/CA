#ifndef MESH_H
#define MESH_H

#include <vector>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class Mesh {
public:
    void buildCube();
    bool init(QOpenGLShaderProgram *program);
    //GL VAO n VBO
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer coordBuffer, indexBuffer, normBuffer;
    void render(QOpenGLFunctions &gl);

private:
    void addVertex(float v0, float v1, float v2);
    void addTriangle(int v0, int v1, int v2);
    void fillVBOs();
    void buildNormals();

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> triangles;
};

#endif // MESH_H
