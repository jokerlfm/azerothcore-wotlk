#ifndef NIER_ACTION_WARLOCK_H
#define NIER_ACTION_WARLOCK_H

#include "NierAction_Base.h"

enum WarlockCurseType :uint32
{
    WarlockCurseType_None = 0,
    WarlockCurseType_Element = 1,
    WarlockCurseType_Weakness = 2,
    WarlockCurseType_Tongues = 3,
};

class NierAction_Warlock :public NierAction_Base
{
public:
    NierAction_Warlock(Player* pmMe);
    void InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex);
    void ResetTalent();
    bool InitializeEquipments(bool pmReset);
    void Prepare();
    bool DPS(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack);
    bool AOE(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing);
    bool Buff(Unit* pmTarget);
    bool Petting(bool pmSummon = true, bool pmReset = false);

    uint32 spell_Shoot;
    uint32 spell_SummonImp;
    uint32 spell_Corruption;
    uint32 spell_Curse_Of_Agony;
    uint32 spell_Immolate;
    uint32 spell_Shadowbolt;
    uint32 spell_RainOfFire;
    uint32 spell_FireShield;
};
#endif
