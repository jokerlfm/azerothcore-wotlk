#include "NingerStrategy_Base.h"
#include "NingerConfig.h"
#include "NingerManager.h"
#include "Group.h"
#include "MotionMaster.h"
#include "Map.h"
#include "Pet.h"

NingerStrategy_Base::NingerStrategy_Base()
{
    me = nullptr;
    initialized = false;

    updateDelay = sNingerConfig->UpdateDelay;
    dpsDelay = sNingerConfig->DPSDelay;
    randomTeleportDelay = 0;

    assembleDelay = 0;
    gatherDelay = 0;
    corpseRunDelay = 0;

    restLimit = 0;
    drinkDelay = 0;

    wanderDuration = 0;
    combatDuration = 0;

    freeze = false;
    cure = true;
    aoe = true;
    petting = true;

    actionLimit = 0;
    ogActionTarget = ObjectGuid::Empty;
    actionType = ActionType::ActionType_None;

    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
    followDistanceMin = 0;
    followDistanceMax = MELEE_MAX_DISTANCE;
}

void NingerStrategy_Base::Report()
{
    if (Group* myGroup = me->GetGroup())
    {
        if (Player* leaderPlayer = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (leaderPlayer->GetGUID() != me->GetGUID())
            {
                me->Whisper("My strategy set to base.", Language::LANG_UNIVERSAL, leaderPlayer);
            }
        }
    }
}

void NingerStrategy_Base::Reset()
{
    assembleDelay = 0;
    gatherDelay = 0;
    corpseRunDelay = 0;

    restLimit = 0;
    drinkDelay = 0;

    wanderDuration = 0;
    combatDuration = 0;

    freeze = false;
    cure = true;
    aoe = true;
    petting = true;

    actionLimit = 0;
    ogActionTarget = ObjectGuid::Empty;
    actionType = ActionType::ActionType_None;

    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
    followDistanceMin = 0;
    followDistanceMax = MELEE_MAX_DISTANCE;
    switch (me->getClass())
    {
    case Classes::CLASS_WARRIOR:
    {
        me->groupRole = GroupRole::GroupRole_Tank;
        break;
    }
    case Classes::CLASS_HUNTER:
    {
        followDistanceMax = FOLLOW_FAR_DISTANCE;
        chaseDistanceMin = RANGE_MIN_DISTANCE;
        chaseDistanceMax = RANGE_NORMAL_DISTANCE;
        break;
    }
    case Classes::CLASS_SHAMAN:
    {
        break;
    }
    case Classes::CLASS_PALADIN:
    {
        break;
    }
    case Classes::CLASS_WARLOCK:
    {
        followDistanceMax = FOLLOW_FAR_DISTANCE;
        chaseDistanceMin = RANGE_MIN_DISTANCE;
        chaseDistanceMax = RANGE_NORMAL_DISTANCE;
        break;
    }
    case Classes::CLASS_PRIEST:
    {
        me->groupRole = GroupRole::GroupRole_Healer;
        followDistanceMax = FOLLOW_FAR_DISTANCE;
        chaseDistanceMin = RANGE_MIN_DISTANCE;
        chaseDistanceMax = RANGE_NORMAL_DISTANCE;
        break;
    }
    case Classes::CLASS_ROGUE:
    {
        break;
    }
    case Classes::CLASS_MAGE:
    {
        followDistanceMax = FOLLOW_FAR_DISTANCE;
        chaseDistanceMin = RANGE_MIN_DISTANCE;
        chaseDistanceMax = RANGE_NORMAL_DISTANCE;
        break;
    }
    case Classes::CLASS_DRUID:
    {
        break;
    }
    default:
    {
        break;
    }
    }
}

void NingerStrategy_Base::Update(uint32 pmDiff)
{
    if (!me)
    {
        return;
    }
    if (!initialized)
    {
        return;
    }
    if (freeze)
    {
        return;
    }
    me->ningerAction->Update(pmDiff);
    if (actionLimit > 0)
    {
        actionLimit -= pmDiff;
        if (actionLimit < 0)
        {
            actionLimit = 0;
        }
        if (Unit* actionTarget = ObjectAccessor::GetUnit(*me, ogActionTarget))
        {
            switch (actionType)
            {
            case ActionType_None:
            {
                break;
            }
            case ActionType_Engage:
            {
                if (Engage(actionTarget))
                {
                    return;
                }
                break;
            }
            case ActionType_Revive:
            {
                if (Revive(actionTarget))
                {
                    return;
                }
                break;
            }                
            default:
                break;
            }
        }
        actionLimit = 0;
        ogActionTarget.Clear();
    }
    if (Group* myGroup = me->GetGroup())
    {
        if (assembleDelay > 0)
        {
            assembleDelay -= pmDiff;
            if (assembleDelay <= 0)
            {
                if (me->IsAlive())
                {
                    if (me->IsBeingTeleported())
                    {
                        assembleDelay = 1000;
                    }
                    else
                    {
                        me->ClearInCombat();
                        me->ningerAction->ClearTarget();
                        if (NingerMovement* nm = me->ningerAction->rm)
                        {
                            nm->ResetMovement();
                        }
                        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                        {
                            if (Player* member = groupRef->GetSource())
                            {
                                if (member->GetGUID() == myGroup->GetLeaderGUID())
                                {
                                    me->Whisper("I am coming.", Language::LANG_UNIVERSAL, member);
                                    me->TeleportTo(member->GetWorldLocation());
                                    return;
                                }
                            }
                        }
                    }
                }
                else
                {
                    assembleDelay = 0;
                }
            }
        }
        if (gatherDelay > 0)
        {
            gatherDelay -= pmDiff;
            if (gatherDelay <= 0)
            {
                gatherDelay = 0;
            }
            return;
        }
        if (GroupInCombat())
        {
            restLimit = 0;
            combatDuration += pmDiff;
            switch (me->groupRole)
            {
            case GroupRole::GroupRole_DPS:
            {
                if (Cure())
                {
                    return;
                }
                if (DPS())
                {
                    return;
                }
                break;
            }
            case GroupRole::GroupRole_Healer:
            {
                if (Heal())
                {
                    return;
                }
                if (Cure())
                {
                    return;
                }
                break;
            }
            case GroupRole::GroupRole_Tank:
            {
                if (Tank())
                {
                    return;
                }
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else
        {
            combatDuration = 0;
            if (restLimit > 0)
            {
                restLimit -= pmDiff;
                if (drinkDelay >= 0)
                {
                    drinkDelay -= pmDiff;
                    if (drinkDelay < 0)
                    {
                        me->ningerAction->Drink();
                    }
                }
                return;
            }
            if (Rest())
            {
                return;
            }
            if (Buff())
            {
                return;
            }
            if (Cure())
            {
                return;
            }
            if (Petting())
            {
                return;
            }
        }
        Follow();
    }
    else
    {
        if (!me->IsAlive())
        {
            if (me->GetDeathTimer() > 60000)
            {
                me->ClearInCombat();
                me->ningerAction->ClearTarget();
                float nearX = 0.0f;
                float nearY = 0.0f;
                float nearZ = 0.0f;
                float nearDistance = frand(40.0f, 50.0f);
                float nearAngle = frand(0.0f, M_PI * 2);
                me->GetNearPoint(me, nearX, nearY, nearZ, me->GetCombatReach(), nearDistance, nearAngle);
                me->ResurrectPlayer(0.2f);
                me->TeleportTo(me->GetMapId(), nearX, nearY, nearZ, 0);
                me->SpawnCorpseBones();
            }
            return;
        }
        if (me->IsInCombat())
        {
            restLimit = 0;
            wanderDuration = 0;
            combatDuration += pmDiff;
            if (Heal())
            {
                return;
            }
            if (Cure())
            {
                return;
            }
            if (DPS(false))
            {
                return;
            }
        }
        else
        {
            combatDuration = 0;
            if (randomTeleportDelay > 0)
            {
                randomTeleportDelay -= pmDiff;
                if (randomTeleportDelay <= 0)
                {
                    randomTeleportDelay = urand(20 * MINUTE * IN_MILLISECONDS, 40 * MINUTE * IN_MILLISECONDS);
                    me->ClearInCombat();
                    me->ningerAction->ClearTarget();
                    me->ningerAction->RandomTeleport();
                }
            }
            if (wanderDuration > 0)
            {
                wanderDuration -= pmDiff;
                if (wanderDuration <= 0)
                {
                    if (Wander())
                    {
                        return;
                    }
                }
            }
        }
    }
}

bool NingerStrategy_Base::GroupInCombat()
{
    if (!me)
    {
        return false;
    }
    if (Group* myGroup = me->GetGroup())
    {
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (member->IsInCombat())
                {
                    if (me->GetDistance(member) < VISIBILITY_DISTANCE_NORMAL)
                    {
                        return true;
                    }
                }
                else if (Pet* memberPet = member->GetPet())
                {
                    if (memberPet->IsInCombat())
                    {
                        if (me->GetDistance(memberPet) < VISIBILITY_DISTANCE_NORMAL)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool NingerStrategy_Base::Engage(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (NingerAction_Base* nab = me->ningerAction)
    {
        switch (me->groupRole)
        {
        case GroupRole::GroupRole_Tank:
        {
            return me->ningerAction->Tank(pmTarget, aoe);
        }
        case GroupRole::GroupRole_DPS:
        {
            return me->ningerAction->DPS(pmTarget, aoe, chaseDistanceMin, chaseDistanceMax);
        }
        default:
        {
            break;
        }
        }
    }

    return false;
}

bool NingerStrategy_Base::Tank(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    if (Group* myGroup = me->GetGroup())
    {
        if (me->groupRole == GroupRole::GroupRole_Tank)
        {
            if (Engage(pmTarget))
            {
                myGroup->SetTargetIcon(7, me->GetGUID(), pmTarget->GetGUID());
                return true;
            }
        }
    }

    return false;
}

bool NingerStrategy_Base::DPS(bool pmDelay)
{
    if (pmDelay)
    {
        if (combatDuration < dpsDelay)
        {
            return false;
        }
    }

    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (Group* myGroup = me->GetGroup())
    {
        if (ObjectGuid ogTankTarget = myGroup->GetGuidByTargetIcon(7))
        {
            if (Unit* tankTarget = ObjectAccessor::GetUnit(*me, ogTankTarget))
            {
                if (me->ningerAction->DPS(tankTarget, aoe, chaseDistanceMin, chaseDistanceMax))
                {
                    return true;
                }
            }
        }

        if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (Unit* leaderTarget = leader->GetSelectedUnit())
            {
                if (me->ningerAction->DPS(leaderTarget, aoe, chaseDistanceMin, chaseDistanceMax))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool NingerStrategy_Base::Tank()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (Group* myGroup = me->GetGroup())
    {
        Unit* ogTankTarget = ObjectAccessor::GetUnit(*me, myGroup->GetGuidByTargetIcon(7));
        if (ogTankTarget)
        {
            if (!ogTankTarget->GetTarget().IsEmpty())
            {
                if (ogTankTarget->GetTarget() != me->GetGUID())
                {
                    if (me->ningerAction->Tank(ogTankTarget, aoe))
                    {
                        return true;
                    }
                }
            }
        }
        Unit* myTarget = me->GetSelectedUnit();
        if (myTarget)
        {
            if (!myTarget->GetTarget().IsEmpty())
            {
                if (myTarget->GetTarget() != me->GetGUID())
                {
                    if (me->ningerAction->Tank(myTarget, aoe))
                    {
                        myGroup->SetTargetIcon(7, me->GetGUID(), myTarget->GetGUID());
                        return true;
                    }
                }
            }
        }
        Unit* nearestOTUnit = nullptr;
        float nearestDistance = VISIBILITY_DISTANCE_NORMAL;
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (member->IsAlive())
                {
                    if (member->GetGUID() != me->GetGUID())
                    {
                        float memberDistance = me->GetDistance(member);
                        if (memberDistance < VISIBILITY_DISTANCE_NORMAL)
                        {
                            std::unordered_set<Unit*> memberAttackers = member->getAttackers();
                            for (std::unordered_set<Unit*>::iterator ait = memberAttackers.begin(); ait != memberAttackers.end(); ++ait)
                            {
                                if (Unit* eachAttacker = *ait)
                                {
                                    if (me->IsValidAttackTarget(eachAttacker))
                                    {
                                        if (!eachAttacker->GetTarget().IsEmpty())
                                        {
                                            if (eachAttacker->GetTarget() != me->GetGUID())
                                            {
                                                float eachDistance = me->GetDistance(eachAttacker);
                                                if (eachDistance < nearestDistance)
                                                {
                                                    if (myGroup->GetTargetIconByGuid(eachAttacker->GetGUID()) == -1)
                                                    {
                                                        nearestDistance = eachDistance;
                                                        nearestOTUnit = eachAttacker;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (nearestOTUnit)
        {
            if (me->ningerAction->Tank(nearestOTUnit, aoe))
            {
                myGroup->SetTargetIcon(7, me->GetGUID(), nearestOTUnit->GetGUID());
                return true;
            }
        }
        if (me->ningerAction->Tank(ogTankTarget, aoe))
        {
            return true;
        }
        if (myTarget)
        {
            if (me->ningerAction->Tank(myTarget, aoe))
            {
                myGroup->SetTargetIcon(7, me->GetGUID(), myTarget->GetGUID());
                return true;
            }
        }
        Unit* nearestAttacker = nullptr;
        nearestDistance = VISIBILITY_DISTANCE_NORMAL;
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (member->IsAlive())
                {
                    if (member->GetGUID() != me->GetGUID())
                    {
                        float memberDistance = me->GetDistance(member);
                        if (memberDistance < VISIBILITY_DISTANCE_NORMAL)
                        {
                            std::unordered_set<Unit*> memberAttackers = member->getAttackers();
                            for (std::unordered_set<Unit*>::iterator ait = memberAttackers.begin(); ait != memberAttackers.end(); ++ait)
                            {
                                if (Unit* eachAttacker = *ait)
                                {
                                    if (me->IsValidAttackTarget(eachAttacker))
                                    {
                                        float eachDistance = me->GetDistance(eachAttacker);
                                        if (eachDistance < nearestDistance)
                                        {
                                            if (myGroup->GetTargetIconByGuid(eachAttacker->GetGUID()) == -1)
                                            {
                                                nearestDistance = eachDistance;
                                                nearestAttacker = eachAttacker;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (nearestAttacker)
        {
            if (me->ningerAction->Tank(nearestAttacker, aoe))
            {
                myGroup->SetTargetIcon(7, me->GetGUID(), nearestAttacker->GetGUID());
                return true;
            }
        }
    }

    return false;
}

bool NingerStrategy_Base::Rest(bool pmForce)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    bool doRest = false;
    if (pmForce)
    {
        doRest = true;
    }
    else
    {
        if (me->GetHealthPct() < 60.0f)
        {
            doRest = true;
        }
        else
        {
            if (me->getPowerType() == Powers::POWER_MANA)
            {
                if (me->GetPowerPct(Powers::POWER_MANA) < 60.0f)
                {
                    doRest = true;
                }
            }
        }
    }

    if (doRest)
    {
        if (me->ningerAction->Eat())
        {
            restLimit = 25 * IN_MILLISECONDS;
            drinkDelay = 1 * IN_MILLISECONDS;
            return true;
        }
    }

    return false;
}

bool NingerStrategy_Base::Heal()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (Group* myGroup = me->GetGroup())
    {
        Player* tank = nullptr;
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (member->groupRole == GroupRole::GroupRole_Tank)
                {
                    tank = member;
                    break;
                }
            }
        }
        if (tank)
        {
            if (me->ningerAction->Heal(tank))
            {
                return true;
            }
        }
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (me->ningerAction->SimpleHeal(member))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        return me->ningerAction->Heal(me);
    }

    return false;
}

bool NingerStrategy_Base::Revive()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (Group* myGroup = me->GetGroup())
    {
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (!member->IsAlive())
                {
                    if (me->ningerAction->Revive(member))
                    {
                        actionLimit = DEFAULT_ACTION_LIMIT_DELAY;
                        ogActionTarget = member->GetGUID();
                        actionType = ActionType::ActionType_Revive;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool NingerStrategy_Base::Revive(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (!pmTarget)
    {
        return false;
    }
    if (Player* targetPlayer = pmTarget->ToPlayer())
    {
        if (targetPlayer->IsAlive())
        {
            return false;
        }
        if (me->ningerAction->Revive(targetPlayer))
        {
            return true;
        }
    }

    return false;
}

bool NingerStrategy_Base::Buff()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (Group* myGroup = me->GetGroup())
    {
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (me->ningerAction->Buff(member))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        return me->ningerAction->Buff(me);
    }

    return false;
}

bool NingerStrategy_Base::Petting()
{
    if (me->ningerAction->Petting(petting))
    {
        return true;
    }

    return false;
}

bool NingerStrategy_Base::Cure()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (Group* myGroup = me->GetGroup())
    {
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (me->ningerAction->Cure(member))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        return me->ningerAction->Cure(me);
    }

    return false;
}

bool NingerStrategy_Base::Follow()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (Group* myGroup = me->GetGroup())
    {
        if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (me->ningerAction->Follow(leader, followDistanceMin, followDistanceMax))
            {
                ogActionTarget = leader->GetGUID();
                return true;
            }
        }
    }

    return false;
}

bool NingerStrategy_Base::Wander()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (me->isMoving())
    {
        return true;
    }

    float angle = frand(0, 2 * M_PI);
    float distance = frand(10.0f, 30.0f);
    float destX = 0.0f, destY = 0.0f, destZ = 0.0f;
    me->GetNearPoint(me, destX, destY, destZ, me->GetCombatReach(), distance, angle);
    wanderDuration = urand(20 * IN_MILLISECONDS, 40 * IN_MILLISECONDS);
    me->ningerAction->rm->MovePoint(destX, destY, destZ, wanderDuration);
    return true;
}

std::string NingerStrategy_Base::GetGroupRole()
{
    if (!me)
    {
        return "";
    }
    switch (me->groupRole)
    {
    case GroupRole::GroupRole_DPS:
    {
        return "dps";
    }
    case GroupRole::GroupRole_Tank:
    {
        return "tank";
    }
    case GroupRole::GroupRole_Healer:
    {
        return "healer";
    }
    default:
    {
        break;
    }
    }
    return "dps";
}

void NingerStrategy_Base::SetGroupRole(std::string pmRoleName)
{
    if (!me)
    {
        return;
    }
    if (pmRoleName == "dps")
    {
        me->groupRole = GroupRole::GroupRole_DPS;
    }
    else if (pmRoleName == "tank")
    {
        me->groupRole = GroupRole::GroupRole_Tank;
    }
    else if (pmRoleName == "healer")
    {
        me->groupRole = GroupRole::GroupRole_Healer;
    }
}
