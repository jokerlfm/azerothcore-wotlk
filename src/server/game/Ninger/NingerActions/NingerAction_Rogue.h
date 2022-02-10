#ifndef NINGER_ACTION_ROGUE_H
#define NINGER_ACTION_ROGUE_H

#include "NingerAction_Base.h"

class NingerAction_Rogue :public NingerAction_Base
{
public:    
    NingerAction_Rogue(Player* pmMe);
    //void Update(uint32 pmDiff);
    //void Reset();
    //bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    //bool DPS_Combat(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    //bool Buff(Unit* pmTarget);

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
