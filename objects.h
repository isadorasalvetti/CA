#ifndef OBJECTS_H
#define OBJECTS_H

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "collider.h"

class Obj {
public:
    //GL VAO n VBO
    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* normBuffer;
    float radius; // -1 = triangle mesh/ > 1 = sphere
    QVector3D color;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> triangles;
};

class Objects {
public:
    std::vector<Obj> myObjs;
    QVector<triangleCollider> triangleColliders;
    Obj sphere;
    Obj triangleMesh;

    bool Init(QOpenGLShaderProgram *program);
    void render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    bool LoadObj(const char *path);
    bool buildBuffers(QOpenGLShaderProgram *program, Obj *m);
    void addColision(QVector<triangleCollider> &vec);

};

#endif // OBJECTS_H
