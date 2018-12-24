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
}

void loadPaladinMeshNMaterial(cal3d::CalCoreModel &myCoreModel){
    int checkN = 0;
    checkN += myCoreModel.loadCoreMesh(characterPath[PLD] + characterString[PLD] + "_body.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[PLD] + characterString[PLD] + "_cape.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[PLD] + characterString[PLD] + "_loincloth.cmf");
    if (checkN < 3) {
        cout << "Mesh loading failed." << endl;
    } checkN = 0;
}

void loadCallyMeshNMaterial(cal3d::CalCoreModel &myCoreModel){
    int checkN = 0;
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_calf_left.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_foot_left.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_hand_left.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_thigh_left.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_upperarm_left.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_lowerarm_left.cmf");
    if (checkN < 6) {
        cout << "Mesh loading failed on left side." << endl;
    } checkN = 0;
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_calf_right.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_foot_right.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_hand_right.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_thigh_right.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_upperarm_right.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_lowerarm_right.cmf");
    if (checkN < 6) {
        cout << "Mesh loading failed on right side." << endl;
    } checkN = 0;
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_pelvis.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_neck.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_ponytail.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_chest.cmf");
    checkN += myCoreModel.loadCoreMesh(characterPath[CALLY] + characterString[CALLY] + "_head.cmf");
    if (checkN < 5) {
        cout << "Mesh loading failed on center." << endl;
    } checkN = 0;
}
