#ifndef NINGER_ACTION_PRIEST_H
#define NINGER_ACTION_PRIEST_H

#include "NingerAction_Base.h"

class NingerAction_Priest :public NingerAction_Base
{
public:
    NingerAction_Priest(Player* pmMe);
    void Update(uint32 pmDiff);
    void Reset();
    //bool Cure(Unit* pmTarget);
    //bool Buff(Unit* pmTarget);
    //bool Revive(Player* pmTarget);
    //bool Heal(Unit* pmTarget);
    //bool Heal_Discipline(Unit* pmTarget);
    //bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);

    void LearnTalents(uint32 pmTabIndex);

    int manaCheckDelay;
    int powerWordShieldDelay;
    int powerWordBarrierDelay;
    int fearWardDelay;
    int painSuppressionDelay;
    int penanceDelay;
    int innerFocusDelay;
    int powerInfusionDelay;
    int hymnOfHopeDelay;
    int prayerOfMendingDelay;
    int shadowfiendDelay;

    int mindBlastDelay;
};
#endif
