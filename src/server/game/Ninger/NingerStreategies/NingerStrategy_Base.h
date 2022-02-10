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
    NingerStrategy_Base(Player* pmMe);
    bool GroupInCombat();

    virtual void Report();
    virtual void Reset();
    virtual void Update(uint32 pmDiff);
    virtual bool Engage(Unit* pmTarget);
    virtual bool DPS(bool pmDelay = true);
    virtual bool Tank();
    virtual bool Tank(Unit* pmTarget);
    virtual bool Rest();
    virtual bool Heal();
    virtual bool Buff();
    virtual bool Cure();
    virtual bool Petting();
    virtual bool Follow();
    virtual bool Wander();
    virtual bool Stay(std::string pmTargetGroupRole);
    virtual bool Hold(std::string pmTargetGroupRole);
    virtual bool Chasing();
    virtual std::string GetGroupRoleName();
    virtual void SetGroupRole(std::string pmRoleName);    

public:
    Player* me;    
    uint32 groupRole;

    ObjectGuid ogEngageTarget;

    bool initialized;

    float chaseDistanceMin;
    float chaseDistanceMax;

    int updateDelay;
    int restDelay;
    int assembleDelay;
    int corpseRunDelay;
    int engageDelay;

    float followDistance;
    float chaseDistance;

    bool staying;
    bool holding;
    bool following;
    bool cure;
    bool aoe;
    bool mark;
    bool petting;
    int assistDelay;
    int reviveDelay;
    int moveDelay;
    int hostilePVPCheckDelay;
    int teleportDelay;
    WorldLocation wlTeleportDestination;
};
#endif
