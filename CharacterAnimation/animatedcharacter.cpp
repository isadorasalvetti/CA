#include "animatedcharacter.h"
#include "ctga.h"

const string characterPath[] = {
    "data/skeleton/",
    "data/cally/",
    "data/paladin/",
};

const string characterString[] = {
    "skeleton",
    "cally",
    "paladin",
};

//----------------------------------------------------------------------------//
// Execute an action of the model                                             //
//----------------------------------------------------------------------------//

void AnimatedCharacter::executeAction(int action){
  switch(action) {
    case 0:
      m_calModel->getMixer()->executeAction(animationId[5], 0.3f, 0.3f);
      break;
    case 1:
      m_calModel->getMixer()->executeAction(animationId[6], 0.3f, 0.3f);
      break;
  }
}

//----------------------------------------------------------------------------//
// Get the lod level of the model                                             //
//----------------------------------------------------------------------------//

float AnimatedCharacter::getLodLevel(){
  return lodLevel;
}

//----------------------------------------------------------------------------//
// Get the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void AnimatedCharacter::getMotionBlend(float *pMotionBlend){
  pMotionBlend[0] = motionBlend[0];
  pMotionBlend[1] = motionBlend[1];
  pMotionBlend[2] = motionBlend[2];
}

//----------------------------------------------------------------------------//
// Get the render scale of the model                                          //
//----------------------------------------------------------------------------//

float AnimatedCharacter::getRenderScale(){
  return renderScale;
}

//----------------------------------------------------------------------------//
// Get the animation state of the model                                       //
//----------------------------------------------------------------------------//

int AnimatedCharacter::getState(){
  return state;
}

void AnimatedCharacter::loadCharacter(QOpenGLFunctions &f, Character c){
    //Set string paths
    string strFilename = characterPath[c] + characterString[c] + ".cfg";
    string strPath = characterPath[c];
    int cnt = 0;

    // open the model configuration file
    std::ifstream file;
    file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
    if(!file) {
        std::cerr << "Failed to open model configuration file '" << strFilename << "'." << std::endl;
    }

    // parse all lines from the model configuration file
    int line;
    for(line = 1; ; line++) {
        // read the next model configuration line
        std::string strBuffer;
        std::getline(file, strBuffer);

        // stop if we reached the end of file
        if(file.eof()) break;

        // check if an error happened while reading from the file
        if(!file) {
            std::cerr << "Error while reading from the model configuration file '" << strFilename << "'." << std::endl;
        }

        // find the first non-whitespace character
        std::string::size_type pos;
        pos = strBuffer.find_first_not_of(" \t");

        // check for empty lines
        if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

        // check for comment lines
        if(strBuffer[pos] == '#') continue;

        // get the key
        std::string strKey;
        strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
        pos += strKey.size();

        // get the '=' character
        pos = strBuffer.find_first_not_of(" \t", pos);
        if((pos == std::string::npos) || (strBuffer[pos] != '=')) {
            std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
        }

        // find the first non-whitespace character after the '=' character
        pos = strBuffer.find_first_not_of(" \t", pos + 1);

        // get the data
        std::string strData;
        strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

        // handle the model creation
        if(strKey == "scale") {
            // set rendering scale factor
            renderScale = atof(strData.c_str());
        }
        else if(strKey == "skeleton") {
                // load core skeleton
                std::cout << "Loading skeleton '" << strData << "'..." << std::endl;
                if(!myCoreModel.loadCoreSkeleton(strPath + strData)) {
                CalError::printLastError();
            }
        }
        else if(strKey == "animation") {
            // load core animation
            std::cout << "Loading animation '" << strData << "'..." << std::endl;
            int animId = myCoreModel.loadCoreAnimation(strPath + strData);
            animationId[cnt] = animId;
            cnt += 1;
            if(!animId) {
                CalError::printLastError();
            }
        }
        else if(strKey == "mesh") {
            // load core mesh
            std::cout << "Loading mesh '" << strData << "'..." << std::endl;
            if(myCoreModel.loadCoreMesh(strPath + strData) == -1) {
                CalError::printLastError();
            }
        }
        else if(strKey == "material") {
          // load core material
            std::cout << "Loading material '" << strData << "'..." << std::endl;
            if(myCoreModel.loadCoreMaterial(strPath + strData) == -1) {
                CalError::printLastError();
            }
        }
        else {
            std::cerr << strFilename << "(" << line << "): Invalid syntax." << std::endl;
        }
    }

    // explicitely close the file
    file.close();

    // load all textures and store the opengl texture id in the corresponding map in the material
    int materialId;
    for(materialId = 0; materialId < myCoreModel.getCoreMaterialCount(); materialId++) {
    // get the core material
    CalCoreMaterial *pCoreMaterial;
    pCoreMaterial = myCoreModel.getCoreMaterial(materialId);

    // loop through all maps of the core material
    int mapId;
    for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++) {
        // get the filename of the texture
        std::string strFilename;
        strFilename = pCoreMaterial->getMapFilename(mapId);

        // load the texture from the file
        GLuint textureId;
        textureId = loadTexture(f, strPath + strFilename);

        // store the opengl texture id in the user data of the map
        pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
    }

    // make one material thread for each material
    // NOTE: this is not the right way to do it, but this viewer can't do the right
    // mapping without further information on the model etc.
    for(materialId = 0; materialId < myCoreModel.getCoreMaterialCount(); materialId++) {
        // create the a material thread
        myCoreModel.createCoreMaterialThread(materialId);

        // initialize the material thread
        myCoreModel.setCoreMaterialId(materialId, 0, materialId);
        }

        // Calculate Bounding Boxes

        myCoreModel.getCoreSkeleton()->calculateBoundingBoxes(&myCoreModel);

        m_calModel = new CalModel(&myCoreModel);

        // attach all meshes to the model
        int meshId;
        for(meshId = 0; meshId < myCoreModel.getCoreMeshCount(); meshId++) {
        m_calModel->attachMesh(meshId);
        }

        // set the material set of the whole model
        m_calModel->setMaterialSet(0);

        // set initial animation state
        state = MOTION;
        m_calModel->getMixer()->blendCycle(animationId[MOTION], motionBlend[0], 0.0f);
        m_calModel->getMixer()->blendCycle(animationId[MOTION + 1], motionBlend[1], 0.0f);
        m_calModel->getMixer()->blendCycle(animationId[MOTION + 2], motionBlend[2], 0.0f);
    }
}

//----------------------------------------------------------------------------//
// Read a int from file stream (to avoid Little/Big endian issue)
//----------------------------------------------------------------------------//

int readInt( std::ifstream *file )
{
    int x = 0;
    for ( int i = 0; i < 32; i+=8 )
    {
        char c;
        file->read ( &c, 1 );
        x += c << i;
    }
    return x;
}

GLuint AnimatedCharacter::loadTexture(QOpenGLFunctions &f, const string& strFilename){
    GLuint pId=0;
    if(stricmp(strrchr(strFilename.c_str(),'.'),".raw")==0)
    {

        // open the texture file
        std::ifstream file;
        file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
        if(!file) {
            std::cerr << "Texture file '" << strFilename << "' not found." << std::endl;
            return 0;
        }

        // load the dimension of the texture
        int width = readInt(&file);
        int height = readInt(&file);
        int depth = readInt(&file);

        // allocate a temporary buffer to load the texture to
        unsigned char *pBuffer;
        pBuffer = new unsigned char[width * height * depth];
        if(pBuffer == nullptr) {
            std::cerr << "Memory allocation for texture '" << strFilename << "' failed." << std::endl;
            return 0;
        }

        // load the texture
        file.read((char *)pBuffer, width * height * depth);

        // explicitely close the file
        file.close();

        // generate texture
        f.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        f.glGenTextures(1, &pId);
        f.glBindTexture(GL_TEXTURE_2D, pId);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        f.glTexImage2D(GL_TEXTURE_2D, 0, (depth == 3) ? GL_RGB : GL_RGBA, width, height, 0, (depth == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, &pBuffer[0]);

        // free the allocated memory
        delete [] pBuffer;

    }
    else if (stricmp(strrchr(strFilename.c_str(),'.'),".tga")==0){
        CTga *Tga;
        Tga = new CTga();

        //Note: This will always make a 32-bit texture
        if(Tga->ReadFile(strFilename.c_str())==0)
        {
            Tga->Release();
            return false;
        }

        //Bind texture
        int width = Tga->GetSizeX();
        int height = Tga->GetSizeY();
        int depth = Tga->Bpp() / 8;

        f.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        f.glGenTextures(1, &pId);

        f.glBindTexture(GL_TEXTURE_2D, pId);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        f.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        f.glTexImage2D(GL_TEXTURE_2D, 0, ((depth == 3) ? GL_RGB : GL_RGBA), width, height, 0, ((depth == 3) ? GL_RGB : GL_RGBA) , GL_UNSIGNED_BYTE, (char*)Tga->GetPointer() );

        Tga->Release();
    }
    return pId;
}

int AnimatedCharacter::getMeshInfo
  (float (&meshVertices)[30000][3], int &vertCount,
   float (&meshNormals)[30000][3], int &normCount,
   float (&meshFaces)[50000][3], int &facesCount,
   const int &meshId, const int &subMeshId){
  m_calModel->update(0.03f);

  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel->getRenderer();

  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return -1;

  // get the number of meshes
  int meshCount = pCalRenderer->getMeshCount();
  int submeshCount = pCalRenderer->getSubmeshCount(meshId);

  // select mesh and submesh for further data access
  if(pCalRenderer->selectMeshSubmesh(meshId, subMeshId)) {

    // get the transformed vertices of the submesh
    static float meshVertices[30000][3];
    int vertexCount;
    vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

    // get the transformed normals of the submesh
    static float meshNormals[30000][3];
    pCalRenderer->getNormals(&meshNormals[0][0]);

    // get the texture coordinates of the submesh
    static float meshTextureCoordinates[30000][2];
    int textureCoordinateCount;
    textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);

    // get the faces of the submesh
    static CalIndex meshFaces[50000][3];
    int faceCount;
    faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);
    }

  // end the rendering
  pCalRenderer->endRendering();
  if (meshId > meshCount) return 1;
  else return 0;

}

//----------------------------------------------------------------------------//
// Shut the model down                                                        //
//----------------------------------------------------------------------------//

void AnimatedCharacter::onShutdown(){
  delete m_calModel;
}

//----------------------------------------------------------------------------//
// Set the lod level of the model                                             //
//----------------------------------------------------------------------------//

void AnimatedCharacter::setLodLevel(float lL){
  lodLevel = lL;

  // set the new lod level in the cal model renderer
  m_calModel->setLodLevel(lodLevel);
}

//----------------------------------------------------------------------------//
// Set the motion blend factors state of the model                            //
//----------------------------------------------------------------------------//

void AnimatedCharacter::setMotionBlend(float *pMotionBlend, float delay){
  motionBlend[0] = pMotionBlend[0];
  motionBlend[1] = pMotionBlend[1];
  motionBlend[2] = pMotionBlend[2];

  m_calModel->getMixer()->clearCycle(animationId[IDLE], delay);
  m_calModel->getMixer()->clearCycle(animationId[FANCY], delay);
  m_calModel->getMixer()->blendCycle(animationId[MOTION], motionBlend[0], delay);
  m_calModel->getMixer()->blendCycle(animationId[MOTION + 1], motionBlend[1], delay);
  m_calModel->getMixer()->blendCycle(animationId[MOTION + 2], motionBlend[2], delay);

  state = MOTION;
}

//----------------------------------------------------------------------------//
// Set a new animation state within a given delay                             //
//----------------------------------------------------------------------------//

void AnimatedCharacter::setState(int st, float delay){
  // check if this is really a new state
  if(state != st)
  {
    if(state == IDLE)
    {
      m_calModel->getMixer()->blendCycle(animationId[IDLE], 1.0f, delay);
      m_calModel->getMixer()->clearCycle(animationId[FANCY], delay);
      m_calModel->getMixer()->clearCycle(animationId[MOTION], delay);
      m_calModel->getMixer()->clearCycle(animationId[MOTION + 1], delay);
      m_calModel->getMixer()->clearCycle(animationId[MOTION + 2], delay);
      state = IDLE;
    }
    else if(state == FANCY)
    {
      m_calModel->getMixer()->clearCycle(animationId[IDLE], delay);
      m_calModel->getMixer()->blendCycle(animationId[FANCY], 1.0f, delay);
      m_calModel->getMixer()->clearCycle(animationId[MOTION], delay);
      m_calModel->getMixer()->clearCycle(animationId[MOTION + 1], delay);
      m_calModel->getMixer()->clearCycle(animationId[MOTION + 2], delay);
      state = FANCY;
    }
    else if(state == MOTION)
    {
      m_calModel->getMixer()->clearCycle(animationId[IDLE], delay);
      m_calModel->getMixer()->clearCycle(animationId[FANCY], delay);
      m_calModel->getMixer()->blendCycle(animationId[MOTION], motionBlend[0], delay);
      m_calModel->getMixer()->blendCycle(animationId[MOTION + 1], motionBlend[1], delay);
      m_calModel->getMixer()->blendCycle(animationId[MOTION + 2], motionBlend[2], delay);
      state = MOTION;
    }
  }
}
