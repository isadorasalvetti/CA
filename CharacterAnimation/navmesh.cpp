#include "navmesh.h"
#include <map>
#include <functional>

const float scl = .7;

void NavMesh::genData(){
   /*
   For defining walls:
   - Check if node (p) is meant to be a wall and has not yet been processed.
     If it is, add it's corrensponding edge coordinates to the coords buffer:
        (p0)(x, y, 0) and (p1)(x, y, z).
        Keep track of index in floor plan to new vert array -> nodeToVert[i*Mx+j] = vertIndx
        (For nodes that have not yet been processed, nodeToVert[i*Mx+j] = -1)
   - Check surrounding nodes.
        If there is a wall to the right and/or bellow (pr): add its edge coords to the buffer. Face is compleeted:
        (Check/ update nodeToVert.)
        face1 = (p1, p0, pr0, pr1)
        face2 = (p0, p1, pr1, pr0) - add both front/back faces.
   Walls to the left and above should have already been created, do not check.
    */

   array<int, Mj*Mi> nodeToVert; //has the vertex fot this node been created?
   array<int, Mj*Mi> nodeToFaces; //This is a closed structure. Each vert should have 2 adjacent faces. Have them been created?
   nodeToVert.fill(-1);
   nodeToFaces.fill(0);

   Mz = Mz*scl;
   for (int i = 0; i < Mi; i++){
     for (int j = 0; j < Mj; j++){
       int cellID = i*Mj+j;
       int cell = floorPlan[cellID];
       if (cell == 1){//generate vertex and faces for the museum walls
       //new verts: Mx*scl, My*scl, Mz
           int face0; int face1;
           if (nodeToVert[cellID] == -1){//add verts if they have not been previously added
           coords.push_back((j-offsetJ)*scl); coords.push_back(0); coords.push_back((-i+offsetI)*scl);
           coords.push_back((j-offsetJ)*scl); coords.push_back(Mz); coords.push_back((-i+offsetI)*scl);
           face0 = coords.size()/3 -2; face1 = coords.size()/3 -1;
           nodeToVert[cellID] = face0; //position of the first cell added in the coords vector.
           }else {//if they have, find its face vert IDs.
              face0 = nodeToVert[cellID]; face1 = nodeToVert[cellID] + 1;
           }
       //Check surrounding faces.
           int btID = (i+1)*Mj+j;
           int cellBottom = floorPlan[btID];

           if (cellBottom == 1){
               int face0b; int face1b;
               if (nodeToVert[btID] == -1){
               coords.push_back((j-offsetJ)*scl); coords.push_back(0); coords.push_back((-i-1+offsetI)*scl);
               coords.push_back((j-offsetJ)*scl); coords.push_back(Mz); coords.push_back((-i-1+offsetI)*scl);
               face0b = coords.size()/3 -2; face1b = coords.size()/3 -1;
               nodeToVert[btID] = face0b; //position of the first cell added in the coords vector.
               } else {face0b = nodeToVert[btID]; face1b = nodeToVert[btID] +1;}

               if(nodeToFaces[btID] < 2){
               //New faces: (face1, face0, face0b, face1b) n (face0, face1, face1b, face0b)
               faces.push_back(face1); faces.push_back(face0); faces.push_back(face1b);
               faces.push_back(face1b); faces.push_back(face0); faces.push_back(face0b);
               nodeToFaces[cellID] += 1;
               nodeToFaces[btID] += 1;}
           }

           if (j == Mj-1) break;
           int rgtID = i*Mj+j+1;
           int cellRight = floorPlan[rgtID];
           if (cellRight == 1){
               int face0r; int face1r;
               if(nodeToVert[rgtID] == -1){
               coords.push_back((j+1-offsetJ)*scl); coords.push_back(0); coords.push_back((-i+offsetI)*scl);
               coords.push_back((j+1-offsetJ)*scl); coords.push_back(Mz); coords.push_back((-i+offsetI)*scl);
               face0r = coords.size()/3 -2; face1r = coords.size()/3 -1;
               nodeToVert[rgtID] = face0r; //position of the first cell added in the coords vector.
               } else {face0r = nodeToVert[rgtID]; face1r = nodeToVert[rgtID] +1;}

               if(nodeToFaces[rgtID] < 2){//new faces, if needed
               //New faces: (face1, face0, face0b, face1b) n (face0, face1, face1b, face0b)
               faces.push_back(face1); faces.push_back(face0); faces.push_back(face1r);
               faces.push_back(face1r); faces.push_back(face0); faces.push_back(face0r);
               nodeToFaces[cellID] += 1;
               nodeToFaces[rgtID] += 1;}
           }
       }//END wall coord condition

       else if (cell == 9){
           possibleObjectives.push_back(iiPair(i, j));
       }
     }}//END Plan loop

   cout<<"End Labyrinth Generation"<<endl;
}

//**************************************************
//Pathifinding
//**************************************************

vector<QVector3D> NavMesh::getPathNObjective(const QVector3D &currPosition){
    //current node from curr postion
    node CurrGridPosition = node(worldToGridPos(currPosition));

    //generate random objective
    node Objective = getRandomObjective();
    while (Objective.coords == CurrGridPosition.coords) Objective = getRandomObjective();

    //generate full path
    vector<iiPair> path = findPath(CurrGridPosition, Objective);

    //convert to coord
    vector<QVector3D> _3DPath;
    _3DPath.reserve(path.size());
    for (iiPair pNode : path){
        _3DPath.push_back(gridToWorldPos(pNode));
    }
    return _3DPath;
}


void computePath(const iiPair &goal, map<iiPair,iiPair> &parent, vector<iiPair> &path) {
    //from FINDPATH
    const iiPair &p = parent[goal];
    if (p != goal) {
        computePath(p, parent, path);
        path.push_back(goal);
    }
}

vector<iiPair> NavMesh::findPath(node originNode, node objectiveNode) {
    auto cmp = [](const node &left, const node &right) {
            return left.cost > right.cost;
    };
    priority_queue<node, vector<node>, decltype(cmp)> toVisit(cmp);

    map<iiPair, iiPair> parentNode;
    // visited => exists in the map

    originNode.cost = 0;
    toVisit.push(originNode);
    parentNode[originNode.coords] = originNode.coords;
    bool goal_found = false;

    while(!toVisit.empty() and not goal_found){
        node currentNode = toVisit.top(); toVisit.pop();
        vector<node> candidates = getNodeNeighboorhoord(currentNode);
        for (node candidate : candidates) {
            if (parentNode.find(candidate.coords) == parentNode.end()){
                candidate.cost = abs(candidate.coords.first - objectiveNode.coords.first) + abs(candidate.coords.second - objectiveNode.coords.second);
                toVisit.push(candidate);
                parentNode[candidate.coords] = currentNode.coords;
            }
            if ((goal_found = candidate.coords == objectiveNode.coords)) {
                vector<iiPair> path;
                computePath(objectiveNode.coords, parentNode, path);
                return path;
            }
        }
    }
    return vector<iiPair>();
}

//Grid functions
int grid2index(const iiPair &p, const iiPair &size) {
    return p.second + p.first * size.second;
}
bool isInGrid(const iiPair &p, const iiPair &size) {
    return p.first >= 0 and p.second >= 0 and
           p.first < size.first and p.second < size.second;
}

vector<node> NavMesh::getNodeNeighboorhoord(node myNode){
    vector<node> neighbors;

    //Ortogonal
    const iiPair size (Mi, Mj);
    const iiPair p2 = myNode.coords;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (not (i == 0 and j == 0)) { // actually moving
                iiPair q = p2;
                q.first += i;
                q.second += j;
                if (isInGrid(q, size))
                    if (floorPlan[grid2index(q, size)] == 0 or floorPlan[grid2index(q, size)] == 9)
                    neighbors.push_back(node(q));
            }
        }
    }

    return neighbors;

}

QVector3D NavMesh::gridToWorldPos(iiPair gridPos){
    return QVector3D((gridPos.second-offsetJ)*scl, 0, (-gridPos.first+offsetI)*scl);
}

iiPair NavMesh::worldToGridPos(QVector3D worldPos){
    float posI = -worldPos.z()/scl+offsetI;
    float posJ = (worldPos.x()/scl)+offsetJ;
    return iiPair((-worldPos.z()/scl+offsetI), (worldPos.x()/scl)+offsetJ);
}

node NavMesh::getRandomObjective(){
    int i = rand()%possibleObjectives.size();
    return possibleObjectives[i];
}
