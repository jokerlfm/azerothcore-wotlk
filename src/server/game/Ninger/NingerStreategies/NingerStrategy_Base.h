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

enum ActionType :uint32
{
    ActionType_None = 0,
    ActionType_Engage = 1,
    ActionType_Revive = 2,
    ActionType_Move = 3,
};

class NingerStrategy_Base
{
public:
    NingerStrategy_Base();

    virtual void Report();
    virtual void Reset();
    virtual void Update(uint32 pmDiff);
    virtual bool Engage(Unit* pmTarget);
    virtual bool Tank(Unit* pmTarget);
    virtual bool Tank();
    virtual bool DPS(bool pmDelay = true);
    virtual bool Heal();
    virtual bool Cure();
    virtual bool Buff();
    virtual bool Revive();
    virtual bool Revive(Unit* pmTarget);
    virtual bool Rest(bool pmForce = false);
    virtual bool Petting();
    virtual bool Follow();
    virtual bool Wander();
    virtual std::string GetGroupRole();
    virtual void SetGroupRole(std::string pmRoleName);    

public:
    Player* me;        
    bool initialized;

    float dpsDistance;
    float followDistance;

    int randomTeleportDelay;

    int corpseRunDelay;
    int dpsDelay;

    int restLimit;
    int drinkDelay;

    int combatDuration;
    int wanderDuration;
    int pvpDelay;

    bool freeze;
    bool cure;
    bool aoe;
    bool petting;
    bool following;
    float combatAngle;

    uint32 actionType;
    int actionLimit;
    ObjectGuid ogActionTarget;

private:
    bool GroupInCombat();

};
#endif
