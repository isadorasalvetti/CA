#include "objects.h"

//bool Objects::LoadObj(const char *path){
//    Obj loadedObj;
//    std::vector<QVector3D> temp_verts;
//    std::vector<QVector3D> temp_normals;
//    std::vector<int> vertexIndices;

//    FILE *file = fopen(path, "r");
//    if( file == NULL ){
//        printf("Impossible to open the file !\n");
//        return false;
//    }
//    while(true){
//        char lineHeader[128];
//        // read the first word of the line
//        int res = fscanf(file, "%s", lineHeader);
//        if (res == EOF) break; // EOF = End Of File. Quit the loop.
//        if ( strcmp( lineHeader, "v" ) == 0 ){
//            float x, y, z;
//            fscanf(file, "%f %f %f\n", &x, &y, &z );
//            QVector3D vertex(x, y, z);
//            temp_verts.push_back(vertex);
//        }
//        else if ( strcmp( lineHeader, "vn" ) == 0 ){
//            float x, y, z;
//            fscanf(file, "%f %f %f\n", &x, &y, &z );
//            QVector3D normal(x, y, z);
//            temp_normals.push_back(normal);
//        }
//        else if ( strcmp( lineHeader, "f" ) == 0 ){
//            std::string vertex1, vertex2, vertex3;
//            int vertexIndex[3];
//            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
//            vertexIndices.push_back(vertexIndex[0]);
//            vertexIndices.push_back(vertexIndex[1]);
//            vertexIndices.push_back(vertexIndex[2]);
//        }
//        else if ( strcmp( lineHeader, "t" ) == 0 ){
//            float tp;
//            fscanf(file, "%f %f %f\n", &tp);
//        }
//    }
//    loadedObj.Indices = vertexIndices;
//    for (unsigned int i = 0; i < temp_verts.size(); i++){
//        loadedObj.verts.push_back(temp_verts[i][0]);
//        loadedObj.verts.push_back(temp_verts[i][1]);
//        loadedObj.verts.push_back(temp_verts[i][2]);
//    }
//    for (unsigned int i = 0; i < temp_normals.size(); i++){
//        loadedObj.normals.push_back(temp_normals[i][0]);
//        loadedObj.normals.push_back(temp_normals[i][1]);
//        loadedObj.normals.push_back(temp_normals[i][2]);
//    }
//    return  true;
//}


bool Objects::Init(QOpenGLShaderProgram *program){
    //Tri Mesh
    float triVertices[] {
        .5, 0, 0,
        0, .5, 0,
        0, 0, .5,
        -.5, 0, 0,
        0, -.5, 0,
        0, 0, -.5
    };

    int faces[]{
        0, 1, 2,
        2, 1, 3,
        3, 1, 5,
        5, 1, 0,
        2, 4, 0,
        3, 4, 2,
        5, 4, 3,
        0, 4, 5,
    };


    float normal[]{
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        -1, 0, 0,
        0, -1, 0,
        0, 0, -1
    };

    for (int i=0; i<sizeof(triVertices)/sizeof(float); i++){
        triangleMesh.vertices.push_back(triVertices[i]);
        triangleMesh.normals.push_back(normal[i]);
    } for (int i=0; i<sizeof(faces)/sizeof(float); i++){
        triangleMesh.triangles.push_back(faces[i]);
    }
    buildBuffers(program, &triangleMesh);
    addColision(triangleColliders);

    return true;
    //Sphere
}

bool Objects::buildBuffers(QOpenGLShaderProgram *program, Obj *m){
    //My Uniforms
    m->color = QVector3D (0.8f, 0.0f, 0.8f);

    //My Buffers
    m->VAO = new QOpenGLVertexArrayObject;

    m->VAO->destroy();
    m->VAO->create();
    if (!m->VAO->isCreated()) return false;
    m->VAO->bind();

    m->coordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    m->coordBuffer->destroy();
    m->coordBuffer->create();
    if (!m->coordBuffer->isCreated()) return false;
    if (!m->coordBuffer->bind()) return false;
    m->coordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m->coordBuffer->allocate(&m->vertices[0], sizeof(float) * m->vertices.size());

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    m->normBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    m->normBuffer->destroy();
    m->normBuffer->create();
    if (!m->normBuffer->isCreated()) return false;
    if (!m->normBuffer->bind()) return false;
    m->normBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m->normBuffer->allocate(&m->normals[0], sizeof(float) * m->normals.size());

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

    m->indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    m->indexBuffer->destroy();
    m->indexBuffer->create();
    if (!m->indexBuffer->isCreated()) return false;
    if (!m->indexBuffer->bind()) return false;
    m->indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m->indexBuffer->allocate(&m->triangles[0], sizeof(int) * m->triangles.size());

    m->VAO->release();
    program->release();
    return true;
}

void Objects::render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program){
    program->setUniformValue("color", triangleMesh.color);
    triangleMesh.VAO->bind();
    gl.glDrawElements(GL_TRIANGLES, triangleMesh.triangles.size(), GL_UNSIGNED_INT, nullptr);
    triangleMesh.VAO->release();
}

void Objects::addColision(QVector<triangleCollider> &vec){
    Obj m = triangleMesh;
    for (unsigned int i=0; i+2<m.triangles.size(); i+= 3){
        QVector3D v1(m.vertices[m.triangles[i] * 3], m.vertices[m.triangles[i] * 3 + 1],
                           m.vertices[m.triangles[i] * 3 + 2]);
        QVector3D v2(m.vertices[m.triangles[i + 1] * 3],
                           m.vertices[m.triangles[i + 1] * 3 + 1],
                           m.vertices[m.triangles[i + 1] * 3 + 2]);
        QVector3D v3(m.vertices[m.triangles[i + 2] * 3],
                           m.vertices[m.triangles[i + 2] * 3 + 1],
                           m.vertices[m.triangles[i + 2] * 3 + 2]);
        QVector3D v1v2 = v2 - v1;
        QVector3D v1v3 = v3 - v1;
        QVector3D normal = QVector3D::crossProduct(v1v2, v1v3).normalized();
        float d = QVector3D::dotProduct(normal, v1);
        QVector3D triCoords (m.triangles[i], m.triangles[i+1], m.triangles[i+2]);
        triangleCollider pl(normal, triCoords, d, 1);
        vec.push_back(pl);
    }
}
