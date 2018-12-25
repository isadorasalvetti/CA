#ifndef ANIMATEDCHARACTER_H
#define ANIMATEDCHARACTER_H

#include "cal3d/cal3d.h"

using namespace std;

enum Character {SKL, CALLY, PLD, ZOMBIES};

class AnimatedCharacter{
public:   
    AnimatedCharacter(): myCoreModel("Character") {}
    CalCoreModel myCoreModel;

private:
    void loadCharacter(Character c = CALLY);
    float renderScale;
};

#endif // ANIMATEDCHARACTER_H
