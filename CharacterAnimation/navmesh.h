#ifndef NAVMESH_H
#define NAVMESH_H

#include <queue>
#include <QVector2D>
#include <QVector3D>
#include <algorithm>
#include <iostream>

using namespace std;
typedef pair<int, int> iiPair;
typedef pair<float, float> ffPair;

//Pathfinding
struct node {
    node(iiPair ii): coords(ii), cost(0) {}
    iiPair coords;
    float cost;
};


class NavMesh{
    public:
    void genData();
    vector<float> coords;
    vector<int> faces;
    vector<node> getNodeNeighboorhoord(node myNode);
    vector<iiPair> findPath(node currentNode, node objectiveNode);
    node getRandomObjective();
    QVector3D gridToWorldPos(iiPair gridPos);
    iiPair worldToGridPos(QVector3D worldPos);

    vector<QVector3D> getPathNObjective(const QVector3D &currPosition);

    static const int Mj = 14;
    static const int Mi = 10;

    const float offsetI = Mi/2.0f;
    const float offsetJ = Mj/2.0f;

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

    private:
    float Mz = 0.5; //arbitrary wall hight
    vector<iiPair> possibleObjectives;
    array<int, Mj*Mi> floorPlan = {
          //0  1  2  3  4  5  6  7  8  9  10 11 12 13
            1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1,   //0
            1, 9, 0, 0, 9, 1, 1, 1, 1, 0, 0, 0, 9, 1,   //1
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   //2
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,   //3
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1,   //4
            1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 9, 1,   //5
            1, 9, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,   //6
            1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 9, 1,   //7
            2, 2, 2, 2, 1, 9, 0, 0, 0, 0, 0, 0, 9, 1,   //8
            2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //9

        }; //14x09 floor plan.

};
#endif // NAVMESH_H
