#ifndef NINGER_STRATEGY_H
#define NINGER_STRATEGY_H

#include "NingerAction_Base.h"

class NingerAction_Base;

enum GroupRole :uint32
{
    GroupRole_DPS = 0,
    GroupRole_Tank = 1,
    GroupRole_Healer = 2,
};

class NingerStrategy_Base
{
public:
    NingerStrategy_Base();

    virtual void Report();
    virtual void Reset();
    virtual void Update(uint32 pmDiff);
    virtual bool Engage(Unit* pmTarget);
    virtual bool Tank();
    virtual bool DPS(bool pmDelay = true);
    virtual bool Heal();
    virtual bool Cure();
    virtual bool Buff();
    virtual bool Revive();
    virtual bool Rest(bool pmForce = false);
    virtual bool Petting();
    virtual bool Follow();
    virtual bool Wander();
    virtual std::string GetGroupRole();
    virtual void SetGroupRole(std::string pmRoleName);    

public:
    Player* me;    
    uint32 groupRole;
    bool initialized;

    float chaseDistanceMin;
    float chaseDistanceMax;    

    float followDistanceMin;
    float followDistanceMax;    

    int updateDelay;
    int randomTeleportDelay;

    int assembleDelay;
    int gatherDelay;
    int corpseRunDelay;
    int dpsDelay;

    int restLimit;
    int drinkDelay;

    int combatDuration;    
    int wanderDuration;

    bool follow;
    bool chase;    
    bool cure;
    bool aoe;
    bool petting;

    int engageLimit;

private:
    bool GroupInCombat();

};
#endif
