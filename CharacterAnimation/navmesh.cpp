#include "navmesh.h"
#include <rendermesh.h>
#include <map>
#include <functional>

static float s = NavMesh::scl/2.5;
static GLfloat vertices[] = {-s, -s, -s,
                      s, -s, -s,
                      s,  s, -s,
                     -s,  s, -s,
                     -s, -s,  s,
                      s, -s,  s,
                      s,  s,  s,
                     -s,  s,  s
                    };

static GLuint faces[] = {3, 1, 0, //z pos
                  3, 2, 1,
                5, 6, 7, //z neg
                  4, 5, 7,
                7, 3, 0, //x pos
                  0, 4, 7,
                1, 2, 6, //x neg
                  6, 5, 1,
                0, 1, 4, //y pos
                  5, 4, 1,
                2, 3, 7, //y neg
                  7, 6, 2
              };


array<int, NavMesh::Mj*NavMesh::Mi> NavMesh::floorPlan = {
          //0  1  2  3  4  5  6  7  8  9  10 11 12 13
            1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1,   //0
            1, 9, 0, 0, 9, 1, 1, 1, 1, 0, 0, 0, 9, 1,   //1
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,   //2
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   //3
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1,   //4
            1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 9, 1,   //5
            1, 9, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,   //6
            1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 1,   //7
            2, 2, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 9, 1,   //8
            2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //9
        }; //14x09 floor plan.

void NavMesh::setProgram(QOpenGLShaderProgram *po){

    for (int i = 0; i < Mi; i++){
        for (int j = 0; j < Mj; j++){
            int cellID = grid2index(iiPair(i, j));
            int cell = floorPlan[cellID];
            if (cell == 9){
                possibleObjectives.push_back(iiPair(i, j));
            }
        }
    }

    p = po;
    p->bind();
    fillBuffers(vertices, faces);
    p->release();
}

void NavMesh::renderMesh(QOpenGLFunctions &gl){
    p->bind();
    VAO.bind();
    p->setUniformValue("color", 0.4f, 0.6f, 0);
    p->setUniformValue("useLight", true);

    QMatrix4x4 model;
    for (int i = 0; i < Mi; i++){
        for (int j = 0; j < Mj; j++){
            int cellID = grid2index(iiPair(i, j));
            int cell = floorPlan[cellID];
            if (cell == 1){
                model.setToIdentity();
                model.scale(QVector3D(1, 0.25, 1));
                model.translate(gridToWorldPos(iiPair(i, j))+QVector3D(0, s/5, 0));
                p->setUniformValue("model", model);
                gl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    VAO.release();
    p->release();
}

void NavMesh::fillBuffers(GLfloat vertices[24], GLuint faces[36]){
    //My Buffers
    VAO.destroy();
    VAO.create();
    if (!VAO.isCreated()) cout << "problem in navmesh vao creation" << endl;
    VAO.bind();

    coordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    coordBuffer->destroy();
    coordBuffer->create();
    if (!coordBuffer->isCreated()) cout << "problem in navmesh buffer creation" << endl;
    if (!coordBuffer->bind()) cout << "problem in navmesh buffer creation" << endl;
    coordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    p->enableAttributeArray(0);
    p->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    normBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    normBuffer->destroy();
    normBuffer->create();
    if (!normBuffer->isCreated()) cout << "problem in navmesh buffer creation" << endl;
    if (!normBuffer->bind()) cout << "problem in navmesh buffer creation" << endl;
    normBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    p->enableAttributeArray(1);
    p->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

    indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indexBuffer->destroy();
    indexBuffer->create();
    if (!indexBuffer->isCreated()) cout << "problem in navmesh index buffer creation" << endl;
    if (!indexBuffer->bind()) cout << "problem in navmesh index buffer creation" << endl;
    indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);

    VAO.release();

    vector<float> normals;
    RenderMesh::buildNormals(vertices, 24, faces, 36, normals);

    //fill buffers
    VAO.bind();
    coordBuffer->bind();
    coordBuffer->allocate(&vertices[0], sizeof(float) * 24);
    normBuffer->bind();
    normBuffer->allocate(&normals[0], sizeof(float) * 36);
    indexBuffer->bind();
    indexBuffer->allocate(&faces[0], sizeof(int) * 36);
//    cout << coordBuffer->size() << endl;
//    cout << indexBuffer->size() << endl;
    VAO.release();
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

    map<iiPair, iiPair> parentNodeForward;
    map<iiPair, iiPair> parentNodeBackward;
    // visited => exists in the map

    originNode.cost = 0;
    toVisit.push(originNode);
    parentNodeForward[originNode.coords] = originNode.coords;
    bool goal_found = false;

    while(!toVisit.empty() and not goal_found){
        //GOING FORWARD
        node currentNode = toVisit.top(); toVisit.pop();
        vector<node> candidates = getNodeNeighboorhoord(currentNode);
        for (node candidate : candidates) {
            if (parentNodeForward.find(candidate.coords) == parentNodeForward.end()){
                candidate.cost = abs(candidate.coords.first - objectiveNode.coords.first) + abs(candidate.coords.second - objectiveNode.coords.second);
                toVisit.push(candidate);
                parentNodeForward[candidate.coords] = currentNode.coords;
            }
            if ((goal_found = candidate.coords == objectiveNode.coords)) {
                vector<iiPair> path;
                computePath(objectiveNode.coords, parentNodeForward, path);
                return path;
            }
        }
    }
    return vector<iiPair>();
}

//Grid functions
int NavMesh::grid2index(const iiPair &p) {
    return p.second + p.first * Mj;
}
bool NavMesh::isInGrid(const iiPair &p) {
    return p.first >= 0 and p.second >= 0 and
           p.first < Mi and p.second < Mj;
}

vector<node> NavMesh::getNodeNeighboorhoord(node myNode){
    vector<node> neighbors;

    const iiPair p2 = myNode.coords;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (not (i == 0 and j == 0) and (i == 0 or j == 0)) { // actually moving
                iiPair q = p2;
                q.first += i;
                q.second += j;
                if (isInGrid(q))
                    if (floorPlan[grid2index(q)] == 0 or floorPlan[grid2index(q)] == 9)
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
    float posI = roundf(-worldPos.z()/scl+offsetI);
    float posJ = roundf((worldPos.x()/scl)+offsetJ);
    return iiPair(posI, posJ);
}

node NavMesh::getRandomObjective(){
    int i = rand()%possibleObjectives.size();
    return possibleObjectives[i];
}
