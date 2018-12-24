#ifndef ANIMATEDCHARACTER_H
#define ANIMATEDCHARACTER_H

#include "cal3d/cal3d.h"

using namespace std;

enum Character {SKL, CALLY, PLD, ZOMBIES};

class AnimatedCharacter{
public:   
    AnimatedCharacter(): myCoreModel("Character") {}
    cal3d::CalCoreModel myCoreModel;

private:
    void loadCharacter(Character c = CALLY);
};

#endif // ANIMATEDCHARACTER_H
