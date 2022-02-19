#ifndef NINGER_ACTION_ROGUE_H
#define NINGER_ACTION_ROGUE_H

#include "NingerAction_Base.h"

class NingerAction_Rogue :public NingerAction_Base
{
public:    
    NingerAction_Rogue();

    int poisonStatus;
    int poisonDelay;
    int kickDelay;    
    int sliceDelay;
    int adrenalineRushDelay;
    int dismantleDelay;
    int killingSpreeDelay;
    int evasionDelay;
    int sprintDelay;
    int revealingStrikeDelay;
    int recuperateDelay;
};
#endif
