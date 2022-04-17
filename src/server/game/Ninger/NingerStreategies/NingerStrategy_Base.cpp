#include "NingerStrategy_Base.h"
#include "NingerConfig.h"
#include "NingerManager.h"
#include "Group.h"
#include "MotionMaster.h"
#include "Map.h"
#include "Pet.h"
#include "GridNotifiers.h"

NingerStrategy_Base::NingerStrategy_Base()
{
    me = nullptr;
    initialized = false;

    dpsDelay = sNingerConfig->DPSDelay;
    randomTeleportDelay = 0;

    corpseRunDelay = 0;

    restLimit = 0;
    drinkDelay = 0;

    wanderDuration = 0;
    combatDuration = 0;
    pvpDelay = 0;

    freeze = false;
    cure = true;
    aoe = true;
    petting = true;
    following = true;

    combatAngle = 0.0f;

    actionLimit = 0;
    ogActionTarget = ObjectGuid::Empty;
    actionType = ActionType::ActionType_None;

    dpsDistance = MELEE_MIN_DISTANCE;
    followDistance = MELEE_MAX_DISTANCE;
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
    corpseRunDelay = 0;

    restLimit = 0;
    drinkDelay = 0;

    wanderDuration = 0;
    combatDuration = 0;
    pvpDelay = 0;

    freeze = false;
    cure = true;
    aoe = true;
    petting = true;
    following = true;

    combatAngle = 0.0f;

    actionLimit = 0;
    ogActionTarget = ObjectGuid::Empty;
    actionType = ActionType::ActionType_None;

    dpsDistance = MELEE_MIN_DISTANCE;
    followDistance = MELEE_MIN_DISTANCE;

    switch (me->getClass())
    {
    case Classes::CLASS_WARRIOR:
    {
        me->groupRole = GroupRole::GroupRole_Tank;
        break;
    }
    case Classes::CLASS_HUNTER:
    {
        followDistance = FOLLOW_NEAR_DISTANCE;
        dpsDistance = RANGE_MIN_DISTANCE;
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
        followDistance = FOLLOW_NEAR_DISTANCE;
        dpsDistance = RANGE_MIN_DISTANCE;
        break;
    }
    case Classes::CLASS_PRIEST:
    {
        me->groupRole = GroupRole::GroupRole_Healer;
        followDistance = FOLLOW_NEAR_DISTANCE;
        dpsDistance = RANGE_MIN_DISTANCE;
        break;
    }
    case Classes::CLASS_ROGUE:
    {
        break;
    }
    case Classes::CLASS_MAGE:
    {
        followDistance = FOLLOW_NEAR_DISTANCE;
        dpsDistance = RANGE_MIN_DISTANCE;
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
    me->ningerMovement->Update(pmDiff);
    me->ningerAction->Update(pmDiff);
    if (actionLimit > 0)
    {
        actionLimit -= pmDiff;
        if (actionLimit < 0)
        {
            actionLimit = 0;
        }
        switch (actionType)
        {
        case ActionType_None:
        {
            break;
        }
        case ActionType_Engage:
        {
            if (Unit* actionTarget = ObjectAccessor::GetUnit(*me, ogActionTarget))
            {
                if (Engage(actionTarget))
                {
                    return;
                }
            }
            break;
        }
        case ActionType_Revive:
        {
            if (Unit* actionTarget = ObjectAccessor::GetUnit(*me, ogActionTarget))
            {
                if (Revive(actionTarget))
                {
                    return;
                }
            }
            break;
        }
        case ActionType::ActionType_Move:
        {
            return;
        }
        default:
            break;
        }
        actionLimit = 0;
        ogActionTarget.Clear();
    }
    if (Group* myGroup = me->GetGroup())
    {
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
            combatAngle = 0.0f;
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
            if (me->GetDeathTimer() > sNingerConfig->ReviveDelay)
            {
                me->ClearInCombat();
                me->ningerAction->ClearTarget();
                me->ningerMovement->ResetMovement();
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
            if (Buff())
            {
                return;
            }
            combatDuration = 0;
            if (randomTeleportDelay > 0)
            {
                randomTeleportDelay -= pmDiff;
                if (randomTeleportDelay <= 0)
                {
                    randomTeleportDelay = urand(20 * MINUTE * IN_MILLISECONDS, 40 * MINUTE * IN_MILLISECONDS);
                    int myClass = me->getClass();
                    if (myClass == Classes::CLASS_HUNTER || myClass == Classes::CLASS_ROGUE)
                    {
                        me->ClearInCombat();
                        me->ningerAction->ClearTarget();
                        me->ningerMovement->ResetMovement();
                        me->ningerAction->RandomTeleport();
                    }
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
            if (pvpDelay >= 0)
            {
                pvpDelay -= pmDiff;
                if (pvpDelay <= 0)
                {
                    pvpDelay = 5000;
                    std::list<Player*> targets;
                    Acore::AnyPlayerInObjectRangeCheck check(me, VISIBILITY_DISTANCE_NORMAL, true);
                    Acore::PlayerListSearcherWithSharedVision<Acore::AnyPlayerInObjectRangeCheck> searcher(me, targets, check);
                    Cell::VisitWorldObjects(me, searcher, VISIBILITY_DISTANCE_NORMAL);
                    for (std::list<Player*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
                    {
                        if (Player* player = (*iter))
                        {
                            if (player->IsHostileTo(me))
                            {
                                if (Engage(player))
                                {
                                    actionLimit = 30 * IN_MILLISECONDS;
                                    ogActionTarget = player->GetGUID();
                                    actionType = ActionType::ActionType_Engage;
                                    break;
                                }
                            }
                        }
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
            if (me->ningerAction->Tank(pmTarget, aoe))
            {
                if (Group* myGroup = me->GetGroup())
                {
                    if (myGroup->GetGuidByTargetIcon(7) != pmTarget->GetGUID())
                    {
                        myGroup->SetTargetIcon(7, me->GetGUID(), pmTarget->GetGUID());
                    }
                }
                return true;
            }
        }
        case GroupRole::GroupRole_DPS:
        {
            return me->ningerAction->DPS(pmTarget, aoe);
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
            if (me->ningerAction->Tank(pmTarget, aoe))
            {
                if (myGroup->GetGuidByTargetIcon(7) != pmTarget->GetGUID())
                {
                    myGroup->SetTargetIcon(7, me->GetGUID(), pmTarget->GetGUID());
                }
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
                if (me->ningerAction->DPS(tankTarget, aoe))
                {
                    return true;
                }
            }
        }

        if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (Unit* leaderTarget = leader->GetSelectedUnit())
            {
                if (leaderTarget->IsInCombat())
                {
                    if (me->ningerAction->DPS(leaderTarget, aoe))
                    {
                        return true;
                    }
                }
            }
        }
    }
    else
    {
        if (Unit* myTarget = me->GetSelectedUnit())
        {
            if (me->ningerAction->DPS(myTarget, aoe))
            {
                return true;
            }
        }
        Unit* nearestAttacker = nullptr;
        float nearestDistance = VISIBILITY_DISTANCE_NORMAL;
        std::unordered_set<Unit*> myAttackers = me->getAttackers();
        for (std::unordered_set<Unit*>::iterator ait = myAttackers.begin(); ait != myAttackers.end(); ++ait)
        {
            if (Unit* eachAttacker = *ait)
            {
                if (me->IsValidAttackTarget(eachAttacker))
                {
                    float eachDistance = me->GetDistance(eachAttacker);
                    if (eachDistance < nearestDistance)
                    {
                        if (!eachAttacker->IsImmunedToDamage(SpellSchoolMask::SPELL_SCHOOL_MASK_ALL))
                        {
                            nearestDistance = eachDistance;
                            nearestAttacker = eachAttacker;
                        }
                    }
                }
            }
        }
        if (nearestAttacker)
        {
            if (me->ningerAction->DPS(nearestAttacker, aoe))
            {
                return true;
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
                                                        if (!eachAttacker->IsImmunedToDamage(SpellSchoolMask::SPELL_SCHOOL_MASK_ALL))
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
        std::unordered_set<Unit*> myAttackers = me->getAttackers();
        for (std::unordered_set<Unit*>::iterator ait = myAttackers.begin(); ait != myAttackers.end(); ++ait)
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
                            if (!eachAttacker->IsImmunedToDamage(SpellSchoolMask::SPELL_SCHOOL_MASK_ALL))
                            {
                                nearestDistance = eachDistance;
                                nearestAttacker = eachAttacker;
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
    if (!following)
    {
        return false;
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
        if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (me->ningerMovement->Follow(leader))
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
    Position dest;
    me->GetNearPoint(me, dest.m_positionX, dest.m_positionY, dest.m_positionZ, me->GetCombatReach(), distance, angle);
    wanderDuration = urand(20 * IN_MILLISECONDS, 40 * IN_MILLISECONDS);
    me->ningerMovement->Point(dest, wanderDuration);
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
