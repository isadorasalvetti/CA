#ifndef MUSEUM_LAYOUT_H
#define MUSEUM_LAYOUT_H

#include "glwidget.h"
#include <algorithm>

using namespace std;
typedef pair<int, int> iiPair;

//Pathfinding
struct node{
    node(iiPair ii): coords(ii), cost(0) {}
    iiPair coords;
    float cost;
    bool operator < (const node &n2){return cost > n2.cost;} //Cheat for priority queue.
    void compCost(const node &objective){
        int h = max(abs(coords.first - objective.coords.first), abs(coords.second - objective.coords.second));
    }
};

class mLayout
{
    public:
    void genData();
    vector<float> coords;
    vector<int> faces;
    vector<node> getNodeNeighboorhoord(node myNode);
    vector<node> findPath(node currentNode, node objectiveNode);
    node getRandomObjective();


    //Update Camera
    QVector3D minCam;
    QVector3D maxCam;

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
    array<int, 14*10> floorPlan = //Must leave a columm of zeroes on the right or left.
        array<int, 14*10>{
          //0  1  2  3  4  5  6  7  8  9 10  11 12 13
            1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,   //0
            1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1,   //1
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   //2
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,   //3
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1,   //4
            1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1,   //5
            1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,   //6
            1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,   //7
            0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,   //8
            0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //9

        }; //14x09 floor plan.

    array<int, 20*22> floorPlanTestMe =
        array<int, 20*22>{
          //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //0
            0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //1
            0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //2
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //3
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //4
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //5
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //6
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //7
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //8
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //9
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //10
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //11
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //12
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //13
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //14
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //15
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //16
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //17
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //18
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //19
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //20
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    //21
        }; //20x22 floor plan.

};
#endif // MUSEUM_LAYOUT_H
