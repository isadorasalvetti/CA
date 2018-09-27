#include "mesh.h"
#include <fstream>
#include <iostream>
#include <QOpenGLFunctions>


void Mesh::addVertex(float v0, float v1, float v2)
{
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
}

void Mesh::addTriangle(int v0, int v1, int v2)
{
    triangles.push_back(v0);
    triangles.push_back(v1);
    triangles.push_back(v2);
}

void Mesh::buildCube()
{
    GLfloat vertices[] = {-1, -1, -1,
                          1, -1, -1,
                          1,  1, -1,
                         -1,  1, -1,
                         -1, -1,  1,
                          1, -1,  1,
                          1,  1,  1,
                         -1,  1,  1
                        };

    GLuint faces[] = {3, 1, 0,
                      3, 2, 1,
                    5, 6, 7,
                      4, 5, 7,
                    7, 3, 0,
                      0, 4, 7,
                    1, 2, 6,
                      6, 5, 1,
                    0, 1, 4,
                      5, 4, 1,
                    2, 3, 7,
                      7, 6, 2
                  };

    int i;
    for(i=0; i<8; i+=1)
        addVertex(0.5f * vertices[3*i], 0.5f * vertices[3*i+1], 0.5f * vertices[3*i+2]);
    for(i=0; i<12; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

void Mesh::buildNormals() {
  const int kFaces = triangles.size();
  std::vector<float> face_normals(kFaces, 0);

  for (int i = 0; i < kFaces; i += 3) {
    QVector3D v1(vertices[triangles[i] * 3], vertices[triangles[i] * 3 + 1],
                       vertices[triangles[i] * 3 + 2]);
    QVector3D v2(vertices[triangles[i + 1] * 3],
                       vertices[triangles[i + 1] * 3 + 1],
                       vertices[triangles[i + 1] * 3 + 2]);
    QVector3D v3(vertices[triangles[i + 2] * 3],
                       vertices[triangles[i + 2] * 3 + 1],
                       vertices[triangles[i + 2] * 3 + 2]);
    QVector3D v1v2 = v2 - v1;
    QVector3D v1v3 = v3 - v1;
    QVector3D normal = QVector3D::crossProduct(v1v2, v1v3);

    normal.normalize();
    for (int j = 0; j < 3; ++j) face_normals[i + j] += normal[j];
}
  const int kVertices = vertices.size();
  normals.resize(kVertices, 0);
  for (int i = 0; i < kFaces; i += 3) {
    for (int j = 0; j < 3; ++j) {
      int idx = triangles[i + j];
      QVector3D v1(vertices[triangles[i + j] * 3],
                         vertices[triangles[i + j] * 3 + 1],
                         vertices[triangles[i + j] * 3 + 2]);
      QVector3D v2(vertices[triangles[i + (j + 1) % 3] * 3],
                         vertices[triangles[i + (j + 1) % 3] * 3 + 1],
                         vertices[triangles[i + (j + 1) % 3] * 3 + 2]);
      QVector3D v3(vertices[triangles[i + (j + 2) % 3] * 3],
                         vertices[triangles[i + (j + 2) % 3] * 3 + 1],
                         vertices[triangles[i + (j + 2) % 3] * 3 + 2]);

      QVector3D v1v2 = v2 - v1;
      QVector3D v1v3 = v3 - v1;
      double angle = acos(QVector3D::dotProduct(v1v2, v1v3) / (v1v2.length() * v1v3.length()));

      for (int k = 0; k < 3; ++k) {
      normals[idx * 3 + k] += face_normals[i + k] * angle;
      }
    }
  }
}

bool Mesh::init(QOpenGLShaderProgram *program)
{
    //Create the mesh and normals
    buildCube();
    buildNormals();

    program->bind();

    coordBuffer.destroy();
    coordBuffer.create();
    if (!coordBuffer.isCreated()) return false;
    coordBuffer.bind();
    coordBuffer.allocate(&vertices[0], 3 * sizeof(float) * vertices.size());
    coordBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    normBuffer.destroy();
    normBuffer.create();
    if (!normBuffer.isCreated()) return false;
    normBuffer.bind();
    normBuffer.allocate(&normals[0], 3 * sizeof(float) * normals.size());
    normBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

    indexBuffer.destroy();
    indexBuffer.create();
    if (!indexBuffer.isCreated()) return false;
    indexBuffer.bind();
    indexBuffer.allocate(&triangles[0], sizeof(int) * triangles.size());
    indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    VAO.release();
    program->release();

    return true;
}

void Mesh::render(QOpenGLFunctions &gl)
{
    VAO.bind();
    indexBuffer.bind();
    gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
    VAO.release();
}
