#ifndef NINGER_ACTION_WARLOCK_H
#define NINGER_ACTION_WARLOCK_H

#include "NingerAction_Base.h"

enum WarlockCurseType :uint32
{
    WarlockCurseType_None = 0,
    WarlockCurseType_Element = 1,
    WarlockCurseType_Weakness = 2,
    WarlockCurseType_Tongues = 3,
};

class NingerAction_Warlock :public NingerAction_Base
{
public:
    NingerAction_Warlock(Player* pmMe);
    void Update(uint32 pmDiff);
    void Reset();
    //bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    //bool DPS_Destruction(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    //bool Buff(Unit* pmTarget);

    uint32 curseType;
    int curseDelay;
    int manaCheckDelay;
    int soulstoneDelay;
    int soulLinkDelay;
    int felArmorDelay;
    int wardDelay;
    int summonDelay;
    int soulHarvestDelay;
    int soulburnDelay;
    int soulFireDelay;
    int soulshatterDelay;
    int shadowfuryDelay;
    int conflagrateDelay;
    int immolateDelay;
    int shadowburnDelay;
    int chaosBoltDelay;
    int baneOfHavocDelay;
    int empoweredImpDelay;
};
#endif