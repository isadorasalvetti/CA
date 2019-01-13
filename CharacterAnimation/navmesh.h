#ifndef NAVMESH_H
#define NAVMESH_H

#include <queue>
#include <QVector2D>
#include <QVector3D>
#include <algorithm>
#include <iostream>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

using namespace std;
typedef pair<int, int> iiPair;
typedef pair<float, float> ffPair;

//Pathfinding
struct node {
    node(iiPair ii): coords(ii), priority(0) {}
    iiPair coords;
    float priority;
};


class NavMesh{
    public:
    void setProgram(QOpenGLShaderProgram *po);
    void renderMesh(QOpenGLFunctions &gl);
    //vector<float> coords;
    //vector<int> faces;
    vector<node> getNodeNeighboorhoord(node myNode);
    vector<iiPair> findPath(node currentNode, node objectiveNode);
    node getRandomObjective();
    static QVector3D gridToWorldPos(iiPair gridPos);
    static iiPair worldToGridPos(QVector3D worldPos);

    vector<QVector3D> getPathNObjective(const QVector3D &currPosition);

    static int grid2index(const iiPair &p);
    static bool isInGrid(const iiPair &p);

    static const int Mj = 14;
    static const int Mi = 10;

    constexpr static const float scl = .9f;
    constexpr static const float offsetI = Mi/2.0f;
    constexpr static const float offsetJ = Mj/2.0f;

    //floor
    vector<float> coordsFloor = {
         0, 0, 0, //0
        13, 0, 0, //1
         0, 0, 9, //2
        13, 0, 9 //3
    };

    vector<int> facesFloor = {
        2, 1, 0,
        3, 1, 2
    };

    vector<float> normalsFloor ={
      0, 1, 0,
      0, 1, 0
    };

    static array<int, Mj*Mi> floorPlan;

    private:
    float Mz = 0.2; //arbitrary wall hight
    vector<iiPair> possibleObjectives;

    void genCube();
    void fillBuffers(GLfloat vertices[], GLuint faces[]);
    QOpenGLShaderProgram* p;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* normBuffer;
    QOpenGLBuffer* indexBuffer;
};
#endif // NAVMESH_H
