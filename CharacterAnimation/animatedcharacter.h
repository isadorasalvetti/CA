#ifndef ANIMATEDCHARACTER_H
#define ANIMATEDCHARACTER_H

#include <QOpenGLFunctions>
#include "cal3d/cal3d.h"

using namespace std;

enum Character {SKL, CALLY, PLD, ZOMBIES};
enum State {IDLE = 0, MOTION=1, FANCY = 2};


class AnimatedCharacter{
public:   
    AnimatedCharacter(): myCoreModel("Character") {
        state = IDLE;
        motionBlend[0] = 0.6f;
        motionBlend[1] = 0.1f;
        motionBlend[2] = 0.3f;
        renderScale = 1.0f;
        lodLevel = 1.0f;
    }
    CalCoreModel myCoreModel;
    CalModel *m_calModel;

    void loadCharacter(QOpenGLFunctions &f, Character c= CALLY);
    void executeAction(int action);
    float getLodLevel();
    void getMotionBlend(float *pMotionBlend);
    float getRenderScale();
    int getState();
    int getMeshInfo(float (&meshVertices)[30000][3], int &vertCount, float (&meshNormals)[30000][3], int &normCount,
    float (&meshFaces)[50000][3], int &facesCount, const int &meshId, const int &subMeshId);
    void onUpdate(float elapsedSeconds);
    void setLodLevel(float lodLevel);
    void setMotionBlend(float *pMotionBlend, float delay);
    void setState(int state, float delay);

    void onShutdown();

private:
    GLuint loadTexture(QOpenGLFunctions &f, const string& strFilename);

    int state;
    float motionBlend[3];
    int animationId[16];
    float lodLevel;
    float renderScale;
};

#endif // ANIMATEDCHARACTER_H
