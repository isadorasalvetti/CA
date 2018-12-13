#include "museum_layout.h"
#include <queue>

const int Mx = 14;
const int My = 10;

void mLayout::genData(){
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

   array<int, Mx*My> nodeToVert; //has the vertex fot this node been created?
   array<int, Mx*My> nodeToFaces; //This is a closed structure. Each vert should have 2 adjacent faces. Have them been created?
   nodeToVert.fill(-1);
   nodeToFaces.fill(0);



   float scl = 1;
   Mz = Mz*scl;
   for (int i = 0; i < My; i++){
       for (int j = 0; j < Mx; j++){
           int cellID = i*Mx+j;
           int cell = floorPlan[cellID];
           if (cell == 1){//generate vertex and faces for the museum walls
           //new verts: Mx*scl, My*scl, Mz
               int face0; int face1;
               if (nodeToVert[cellID] == -1){//add verts if they have not been previously added
               coords.push_back(j*scl); coords.push_back(0); coords.push_back(-i*scl);
               coords.push_back(j*scl); coords.push_back(Mz); coords.push_back(-i*scl);
               face0 = coords.size()/3 -2; face1 = coords.size()/3 -1;
               nodeToVert[cellID] = face0; //position of the first cell added in the coords vector.
               }else {//if they have, find its face vert IDs.
                  face0 = nodeToVert[cellID]; face1 = nodeToVert[cellID] + 1;
               }
           //Check surrounding faces.
               int btID = (i+1)*Mx+j;
               int cellBottom = floorPlan[btID];

               if (cellBottom == 1){
                   int face0b; int face1b;
                   if (nodeToVert[btID] == -1){
                   coords.push_back(j*scl); coords.push_back(0); coords.push_back(-(i+1)*scl);
                   coords.push_back(j*scl); coords.push_back(Mz); coords.push_back(-(i+1)*scl);
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

               if (j == Mx-1) break;
               int rgtID = i*Mx+j+1;
               int cellRight = floorPlan[rgtID];
               if (cellRight == 1){
                   int face0r; int face1r;
                   if(nodeToVert[rgtID] == -1){
                   coords.push_back((j+1)*scl); coords.push_back(0); coords.push_back(-i*scl);
                   coords.push_back((j+1)*scl); coords.push_back(Mz); coords.push_back(-i*scl);
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
               //Build Min/ Max to set camera at spawn
               minCam = QVector3D (j - Mz/5, 1.0, i - Mz/5);
               maxCam = QVector3D (j + Mz/5, 0.5, i + Mz/5);

           }
       }}//END Plan loop

   cout<<"End Labyrinth Generation"<<endl;
}

//**************************************************
//Pathifinding
//**************************************************


//Grid functions

int grid2index(iiPair &p, iiPair &size) {
    return p.second + p.first * size.second;
}
bool isInGrid(iiPair &p, iiPair &size) {
    return p.first >= 0 and p.second >= 0 and
           p.first < size.first and p.second < size.second;
}



void mLayout::findPath(node currentNode, node objectiveNode){
    priority_queue<node, vector<node>, less<node>> openList;
    vector<node> closedList;

    openList.push(currentNode);

    while(!openList.empty()){
        node q = openList.top();
        vector<node> candidates = getNodeNeighboorhoord(q);
        for (unsigned int i = 0; i<candidates.size(); i++){
            if (candidates[i].coords == objectiveNode) break; //this node is the goal.
        }
    }

}

vector<node> mLayout::getNodeNeighboorhoord(node myNode){
    vector<node> neighbors;
    iiPair p = myNode.coords;

    //Ortogonal
    iiPair p2 = p;
    p2.first += 1;
    if (isInGrid(p2, size) //coord must be valid
     && floorPlan[grid2index(p2, iiPair(Mx, My))]) //coord must be walkable
        neighbors.append(node(p2)); //append node to candidate list
    p2 = p;
    p2.first -= 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));
    p2 = p;
    p2.second -= 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));
    p2 = p;
    p2.second += 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));

    //Diagonals
    p2 = p;
    p2.first += 1;
    p2.second += 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));
    p2 = p;
    p2.first -= 1;
    p2.second += 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));
    p2 = p;
    p2.first += 1;
    p2.second -= 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));
    p2 = p;
    p2.first -= 1;
    p2.second -= 1;
    if (isInGrid(p2, size)
     && floorPlan[grid2index(p2, iiPair(Mx, My))])
        neighbors.append(node(p2));

}

node mLayout::getRandomObjective(){

}
