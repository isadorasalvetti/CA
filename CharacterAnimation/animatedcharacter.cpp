#include "animatedcharacter.h"

const string characterPath[] = {
    "characters/skeleton/",
    "characters/cally/",
    "characters/paladin/",
};

const string characterString[] = {
    "skeleton",
    "cally",
    "paladin",
};
/*
void AnimatedCharacter::loadCharacter(Character c){
    if(!myCoreModel.loadCoreSkeleton(characterPath[c] + characterString[c] + ".csf")) cout << "Character failed to load." << c << endl;
    string s;
    int idleAnimationId, walkAnimationId, waveAnimationId;
    idleAnimationId = myCoreModel.loadCoreAnimation(characterPath[c] + characterString[c] + "_idle.caf");
    walkAnimationId = myCoreModel.loadCoreAnimation(characterPath[c] + characterString[c] + "_walk.caf");
    waveAnimationId = myCoreModel.loadCoreAnimation(characterPath[c] + characterString[c] + "_wave.caf");

    if((idleAnimationId == -1) || (walkAnimationId == -1) || (waveAnimationId == -1)){
      cout << "Animation failed to load." << c << endl;
      cout << "Ids:" << idleAnimationId << ", " << walkAnimationId << ", " << waveAnimationId << ".";
    }
}*/

void AnimatedCharacter::loadCharacter(Character c){
  //Set string paths
  string strFilename = characterPath[c] + characterString[c] + ".csf";
  string strPath = characterPath[c];

  // open the model configuration file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);
  if(!file) {
    std::cerr << "Failed to open model configuration file '" << strFilename << "'." << std::endl;
  }

  // initialize the animation count
  int animationCount;
  animationCount = 0;

  // parse all lines from the model configuration file
  int line;
  for(line = 1; ; line++)
  {
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
      if(myCoreModel.loadCoreSkeleton(strPath + strData)) {
        CalError::printLastError();
      }
    }
    else if(strKey == "animation") {
      // load core animation
      std::cout << "Loading animation '" << strData << "'..." << std::endl;
      if(myCoreModel.loadCoreAnimation(strPath + strData)) {
        CalError::printLastError();
      }

      animationCount++;
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
/*
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
      textureId = loadTexture(strPath + strFilename);

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

  myCoreModel.getCoreSkeleton()->calculateBoundingBoxes(m_calCoreModel);

  m_calModel = new CalModel(m_calCoreModel);

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < myCoreModel.getCoreMeshCount(); meshId++) {
    m_calModel->attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel->setMaterialSet(0);

  // set initial animation state
  m_state = STATE_MOTION;
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION], m_motionBlend[0], 0.0f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 1], m_motionBlend[1], 0.0f);
  m_calModel->getMixer()->blendCycle(m_animationId[STATE_MOTION + 2], m_motionBlend[2], 0.0f);
*/
}
