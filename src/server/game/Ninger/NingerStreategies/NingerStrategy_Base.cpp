#include "NingerStrategy_Base.h"
#include "NingerConfig.h"
#include "NingerManager.h"
#include "Group.h"
#include "MotionMaster.h"
#include "Map.h"
#include "Pet.h"

/*
NingerStrategy::NingerStrategy(Player* pmMe)
{
    initialized = false;
    me = pmMe;
    groupRole = GroupRole::GroupRole_DPS;
    ogEngageTarget = ObjectGuid::Empty;
    teleportDelay = 0;    
    reviveDelay = 0;
    engageDelay = 0;
    moveDelay = 0;
    hostilePVPCheckDelay = 0;
    staying = false;
    holding = false;
    following = true;
    cure = true;
    aoe = true;
    mark = false;
    petting = true;
    followDistance = FOLLOW_MIN_DISTANCE;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
    switch (me->getClass())
    {
    case Classes::CLASS_WARRIOR:
    {
        sb = new Script_Warrior(me);
        break;
    }
    case Classes::CLASS_HUNTER:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMin = FOLLOW_NORMAL_DISTANCE;
        chaseDistanceMax = FOLLOW_FAR_DISTANCE;
        sb = new Script_Hunter(me);
        break;
    }
    case Classes::CLASS_SHAMAN:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        sb = new Script_Shaman(me);
        break;
    }
    case Classes::CLASS_PALADIN:
    {
        followDistance = FOLLOW_NORMAL_DISTANCE;
        sb = new Script_Paladin(me);
        break;
    }
    case Classes::CLASS_WARLOCK:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        sb = new Script_Warlock(me);
        break;
    }
    case Classes::CLASS_PRIEST:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        sb = new Script_Priest(me);
        break;
    }
    case Classes::CLASS_ROGUE:
    {
        sb = new Script_Rogue(me);
        break;
    }
    case Classes::CLASS_MAGE:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        sb = new Script_Mage(me);
        break;
    }
    case Classes::CLASS_DRUID:
    {
        sb = new Script_Druid(me);
        break;
    }
    default:
    {
        sb = new Script_Base(me);
        break;
    }
    }
}

void NingerStrategy::Report()
{
    if (Group* myGroup = me->GetGroup())
    {
        if (Player* leaderPlayer = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (leaderPlayer->GetGUID() != me->GetGUID())
            {
                sNingerManager->WhisperTo(leaderPlayer, "My awareness set to base.", Language::LANG_UNIVERSAL, me);
            }
        }
    }
}

void NingerStrategy::Reset()
{
    groupRole = GroupRole::GroupRole_DPS;
    ogEngageTarget = ObjectGuid::Empty;
    randomTeleportDelay = urand(20 * MINUTE * IN_MILLISECONDS, 30 * MINUTE * IN_MILLISECONDS);
    deadTime = 0;
    teleportDelay = 0;
    reviveDelay = 0;
    engageDelay = 0;
    moveDelay = 0;
    combatTime = 0;
    teleportAssembleDelay = 0;
    resurrectDelay = 0;
    eatDelay = 0;
    drinkDelay = 0;
    readyCheckDelay = 0;
    hostilePVPCheckDelay = 0;
    staying = false;
    holding = false;
    following = true;
    cure = true;
    aoe = true;
    mark = false;
    petting = true;
    dpsDelay = sNingerConfig->DPSDelay;
    followDistance = FOLLOW_MIN_DISTANCE;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
    sb->Reset();
    switch (me->getClass())
    {
    case Classes::CLASS_WARRIOR:
    {
        break;
    }
    case Classes::CLASS_HUNTER:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMin = FOLLOW_NORMAL_DISTANCE;
        chaseDistanceMax = FOLLOW_FAR_DISTANCE;
        break;
    }
    case Classes::CLASS_SHAMAN:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        break;
    }
    case Classes::CLASS_PALADIN:
    {
        followDistance = FOLLOW_NORMAL_DISTANCE;
        break;
    }
    case Classes::CLASS_WARLOCK:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        break;
    }
    case Classes::CLASS_PRIEST:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
        break;
    }
    case Classes::CLASS_ROGUE:
    {
        break;
    }
    case Classes::CLASS_MAGE:
    {
        followDistance = FOLLOW_FAR_DISTANCE;
        chaseDistanceMax = RANGE_DPS_DISTANCE;
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

bool NingerStrategy::Chasing()
{
    if (holding)
    {
        return false;
    }
    return true;
}

void NingerStrategy::Update(uint32 pmDiff)
{
    if (!me)
    {
        return;
    }
    if (!initialized)
    {
        return;
    }
    if (WorldSession* mySesson = me->GetSession())
    {
        if (mySesson->isNinger)
        {
            sb->Update(pmDiff);
            if (teleportDelay > 0)
            {
                teleportDelay -= pmDiff;
                if (teleportDelay <= 0)
                {
                    if (me->IsBeingTeleported())
                    {
                        teleportDelay = 1000;                        
                    }
                    else
                    {
                        me->ClearInCombat();
                        sb->ClearTarget();
                        sb->rm->ResetMovement();
                        me->TeleportTo(wlTeleportDestination);
                        return;
                    }                    
                }
            }
            if (Group* myGroup = me->GetGroup())
            {
                if (readyCheckDelay > 0)
                {
                    readyCheckDelay -= pmDiff;
                    if (readyCheckDelay <= 0)
                    {
                        if (Player* leaderPlayer = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
                        {
                            if (WorldSession* leaderWS = leaderPlayer->GetSession())
                            {
                                if (!leaderWS->isNinger)
                                {
                                    uint8 readyCheckValue = 0;
                                    if (!me->IsAlive())
                                    {
                                        readyCheckValue = 0;
                                    }
                                    else if (me->GetDistance(leaderPlayer) > VISIBILITY_DISTANCE_NORMAL)
                                    {
                                        readyCheckValue = 0;
                                    }
                                    else
                                    {
                                        readyCheckValue = 1;
                                    }
                                    WorldPacket data(MSG_RAID_READY_CHECK, 8);
                                    data << readyCheckValue;
                                    if (WorldSession* myWS = me->GetSession())
                                    {
                                        myWS->HandleRaidReadyCheckOpcode(data);
                                    }
                                }
                            }
                        }
                    }
                }
                if (staying)
                {
                    return;
                }
                if (teleportAssembleDelay > 0)
                {
                    teleportAssembleDelay -= pmDiff;
                    if (teleportAssembleDelay <= 0)
                    {
                        teleportAssembleDelay = 0;
                        Player* leaderPlayer = nullptr;
                        bool canTeleport = true;
                        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                        {
                            if (Player* member = groupRef->GetSource())
                            {
                                if (member->GetGUID() == myGroup->GetLeaderGUID())
                                {
                                    leaderPlayer = member;
                                }
                                if (member->IsBeingTeleported())
                                {
                                    sNingerManager->WhisperTo(leaderPlayer, "Some one is teleporting. I will wait.", Language::LANG_UNIVERSAL, me);
                                    teleportAssembleDelay = 8000;
                                    canTeleport = false;
                                    break;
                                }
                            }
                        }
                        if (canTeleport)
                        {
                            if (leaderPlayer)
                            {
                                if (leaderPlayer->IsInWorld())
                                {
                                    sNingerManager->TeleportPlayer(me, leaderPlayer->GetMapId(), leaderPlayer->GetPositionX(), leaderPlayer->GetPositionY(), leaderPlayer->GetPositionZ());
                                    if (me->IsAlive())
                                    {
                                        sNingerManager->WhisperTo(leaderPlayer, "I am coming.", Language::LANG_UNIVERSAL, me);
                                    }
                                    else
                                    {
                                        resurrectDelay = urand(10000, 15000);
                                        sNingerManager->WhisperTo(leaderPlayer, "I am coming. And I will revive in a few seconds", Language::LANG_UNIVERSAL, me);
                                    }
                                }
                                else
                                {
                                    sNingerManager->WhisperTo(leaderPlayer, "Leader is not in world", Language::LANG_UNIVERSAL, me);
                                }
                            }
                            else
                            {
                                sNingerManager->WhisperTo(leaderPlayer, "Can not find leader", Language::LANG_UNIVERSAL, me);
                            }
                            return;
                        }
                    }
                }
                if (!me->IsAlive())
                {
                    deadTime += pmDiff;
                    return;
                }
                else
                {
                    deadTime = 0;
                }
                if (engageDelay > 0)
                {
                    engageDelay -= pmDiff;
                    if (engageDelay <= 0)
                    {
                        ogEngageTarget.Clear();
                        sb->rm->ResetMovement();
                        sb->ClearTarget();
                        return;
                    }
                    if (Unit* engageTarget = ObjectAccessor::GetUnit(*me, ogEngageTarget))
                    {
                        switch (groupRole)
                        {
                        case GroupRole::GroupRole_DPS:
                        {
                            if (sb->DPS(engageTarget, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
                            {
                                return;
                            }
                            else
                            {
                                ogEngageTarget.Clear();
                                sb->rm->ResetMovement();
                                sb->ClearTarget();
                                engageDelay = 0;
                            }
                            break;
                        }
                        case GroupRole::GroupRole_Healer:
                        {
                            if (Heal())
                            {
                                return;
                            }
                            break;
                        }
                        case GroupRole::GroupRole_Tank:
                        {
                            if (sb->Tank(engageTarget, Chasing(), aoe))
                            {
                                return;
                            }
                            else
                            {
                                ogEngageTarget.Clear();
                                sb->rm->ResetMovement();
                                sb->ClearTarget();
                                engageDelay = 0;
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                    }
                    return;
                }
                if (resurrectDelay > 0)
                {
                    resurrectDelay -= pmDiff;
                    if (resurrectDelay < 0)
                    {
                        resurrectDelay = 0;
                        if (!me->IsAlive())
                        {
                            me->ResurrectPlayer(0.2f);
                            me->SpawnCorpseBones();
                        }
                    }
                }
                if (reviveDelay > 0)
                {
                    reviveDelay -= pmDiff;
                    if (!sb->Revive(nullptr))
                    {
                        reviveDelay = 0;
                        sb->ogReviveTarget.Clear();
                    }
                    if (reviveDelay <= 0)
                    {
                        sb->ogReviveTarget.Clear();
                    }
                    return;
                }
                if (assistDelay > 0)
                {
                    assistDelay -= pmDiff;
                    if (sb->Assist(nullptr))
                    {
                        return;
                    }
                    else
                    {
                        assistDelay = 0;
                    }
                }
                if (moveDelay > 0)
                {
                    moveDelay -= pmDiff;
                    if (moveDelay < 0)
                    {
                        moveDelay = 0;
                    }
                    return;
                }
                if (GroupInCombat())
                {
                    eatDelay = 0;
                    drinkDelay = 0;
                    combatTime += pmDiff;
                    if (sb->Assist(nullptr))
                    {
                        return;
                    }
                    switch (groupRole)
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
                    combatTime = 0;
                    if (eatDelay > 0)
                    {
                        eatDelay -= pmDiff;
                        if (drinkDelay > 0)
                        {
                            drinkDelay -= pmDiff;
                            if (drinkDelay <= 0)
                            {
                                sb->Drink();
                            }
                        }
                        return;
                    }
                    switch (groupRole)
                    {
                    case GroupRole::GroupRole_DPS:
                    {
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
                        break;
                    }
                    case GroupRole::GroupRole_Healer:
                    {
                        if (Rest())
                        {
                            return;
                        }
                        if (Heal())
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
                        break;
                    }
                    case GroupRole::GroupRole_Tank:
                    {
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
                        break;
                    }
                    default:
                    {
                        break;
                    }
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
                    deadTime += pmDiff;
                    if (deadTime > 60000)
                    {
                        if (sNingerManager->RandomTeleport(me))
                        {
                            Reset();
                            return;
                        }
                    }
                    return;
                }
                else
                {
                    deadTime = 0;
                }
                if (me->IsInCombat())
                {
                    engageDelay = 0;
                    moveDelay = 0;
                    eatDelay = 0;
                    drinkDelay = 0;
                    combatTime += pmDiff;
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
                    combatTime = 0;
                    if (engageDelay > 0)
                    {
                        engageDelay -= pmDiff;
                        if (engageDelay <= 0)
                        {
                            ogEngageTarget.Clear();
                            sb->rm->ResetMovement();
                            sb->ClearTarget();
                            return;
                        }
                        if (Unit* engageTarget = ObjectAccessor::GetUnit(*me, ogEngageTarget))
                        {
                            if (sb->DPS(engageTarget, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
                            {
                                return;
                            }
                            else
                            {
                                ogEngageTarget.Clear();
                                sb->rm->ResetMovement();
                                sb->ClearTarget();
                                engageDelay = 0;
                            }
                        }
                        return;
                    }
                    if (randomTeleportDelay > 0)
                    {
                        randomTeleportDelay -= pmDiff;
                        if (randomTeleportDelay <= 0)
                        {
                            randomTeleportDelay = 5000;
                            if (sNingerManager->RandomTeleport(me))
                            {
                                randomTeleportDelay = urand(20 * MINUTE * IN_MILLISECONDS, 30 * MINUTE * IN_MILLISECONDS);
                                Reset();
                                return;
                            }
                        }
                    }
                    // hostile pvp check
                    if (hostilePVPCheckDelay >= 0)
                    {
                        hostilePVPCheckDelay -= pmDiff;
                        if (hostilePVPCheckDelay < 0)
                        {
                            hostilePVPCheckDelay = 1000;
                            if (Player* enemyPlayer = sNingerManager->GetNearbyHostilePlayer(me))
                            {
                                if (sb->DPS(enemyPlayer, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
                                {
                                    ogEngageTarget = enemyPlayer->GetGUID();
                                    engageDelay = 30 * IN_MILLISECONDS;
                                    return;
                                }
                            }
                            if (me->GetAreaId() == 3628)
                            {
                                // attack any hostile unit
                                if (Unit* enemyUnit = sNingerManager->GetNearbyHostileUnit(me))
                                {
                                    if (sb->DPS(enemyUnit, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
                                    {
                                        ogEngageTarget = enemyUnit->GetGUID();
                                        engageDelay = 30 * IN_MILLISECONDS;
                                        return;
                                    }
                                }
                            }
                        }
                    }
                    if (moveDelay > 0)
                    {
                        moveDelay -= pmDiff;
                        if (moveDelay <= 0)
                        {
                            moveDelay = 0;
                        }
                        return;
                    }
                    if (eatDelay > 0)
                    {
                        eatDelay -= pmDiff;
                        if (drinkDelay > 0)
                        {
                            drinkDelay -= pmDiff;
                            if (drinkDelay <= 0)
                            {
                                sb->Drink();
                            }
                        }
                        return;
                    }
                    sb->ClearTarget();
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
                    if (Wander())
                    {
                        return;
                    }
                }
            }
        }
    }
}

bool NingerStrategy::GroupInCombat()
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
                    if (memberPet->IsAlive())
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
    }

    return false;
}

bool NingerStrategy::Engage(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    switch (groupRole)
    {
    case GroupRole::GroupRole_Tank:
    {
        return sb->Tank(pmTarget, Chasing(), aoe);
    }
    case GroupRole::GroupRole_DPS:
    {
        return sb->DPS(pmTarget, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax);
    }
    case GroupRole::GroupRole_Healer:
    {
        return Heal();
    }
    default:
    {
        break;
    }
    }

    return false;
}

bool NingerStrategy::DPS(bool pmDelay)
{
    if (pmDelay)
    {
        if (combatTime < dpsDelay)
        {
            return false;
        }
    }

    return sb->DPS(nullptr, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax);
}

bool NingerStrategy::Tank()
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (Group* myGroup = me->GetGroup())
    {
        // icon ot 
        if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetGuidByTargetIcon(7)))
        {
            if (!target->GetTarget().IsEmpty())
            {
                if (target->GetTarget() != me->GetGUID())
                {
                    for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                    {
                        if (Player* member = groupRef->GetSource())
                        {
                            if (target->GetTarget() == member->GetGUID())
                            {
                                if (sb->Tank(target, Chasing(), aoe))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        // ot 
        Unit* nearestAttacker = nullptr;
        float nearestDistance = VISIBILITY_DISTANCE_NORMAL;
        for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (member->IsAlive())
                {
                    std::unordered_set<Unit*> memberAttackers = member->getAttackers();
                    for (std::unordered_set<Unit*>::iterator ait = memberAttackers.begin(); ait != memberAttackers.end(); ++ait)
                    {
                        if (Unit* eachAttacker = *ait)
                        {
                            if (eachAttacker->IsAlive())
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
        }
        if (nearestAttacker)
        {
            if (sb->Tank(nearestAttacker, Chasing(), aoe))
            {
                myGroup->SetTargetIcon(7, me->GetGUID(), nearestAttacker->GetGUID());
                return true;
            }
        }
        // icon 
        if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetGuidByTargetIcon(7)))
        {
            if (sb->Tank(target, Chasing(), aoe))
            {
                return true;
            }
        }
    }

    return false;
}

bool NingerStrategy::Tank(Unit* pmTarget)
{
    return sb->Tank(pmTarget, Chasing(), aoe);
}

bool NingerStrategy::Rest()
{
    if (sb->Eat())
    {
        eatDelay = DEFAULT_REST_DELAY;
        drinkDelay = 1000;
        return true;
    }

    return false;
}

bool NingerStrategy::Heal()
{
    if (sb->Heal(nullptr))
    {
        return true;
    }

    return false;
}

bool NingerStrategy::Buff()
{
    if (sb->Buff(nullptr))
    {
        return true;
    }

    return false;
}

bool NingerStrategy::Petting()
{
    if (sb->Petting(petting))
    {
        return true;
    }

    return false;
}

bool NingerStrategy::Cure()
{
    if (cure)
    {
        if (sb->Cure(nullptr))
        {
            return true;
        }
    }

    return false;
}

bool NingerStrategy::Follow()
{
    if (holding)
    {
        return false;
    }
    if (!following)
    {
        return false;
    }
    if (Group* myGroup = me->GetGroup())
    {
        if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
        {
            if (sb->Follow(leader, followDistance))
            {
                return true;
            }
        }
    }

    return false;
}

bool NingerStrategy::Wander()
{
    if (me->isMoving())
    {
        return true;
    }

    uint32 wanderRate = urand(0, 100);
    if (wanderRate < 50)
    {
        if (Player* enemyPlayer = sNingerManager->GetNearbyHostilePlayer(me, VISIBILITY_DISTANCE_NORMAL))
        {
            if (sb->DPS(enemyPlayer, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
            {
                ogEngageTarget = enemyPlayer->GetGUID();
                engageDelay = 30 * IN_MILLISECONDS;
                return true;
            }
        }
    }
    else
    {
        float angle = frand(0, 2 * M_PI);
        float distance = frand(10.0f, 30.0f);
        float destX = 0.0f, destY = 0.0f, destZ = 0.0f;
        me->GetNearPoint(me, destX, destY, destZ, me->GetCombatReach(), distance, angle);
        moveDelay = 5 * IN_MILLISECONDS;        
        sb->rm->MovePoint(destX, destY, destZ, moveDelay);
    }

    return true;
}

bool NingerStrategy::Stay(std::string pmTargetGroupRole)
{
    bool todo = true;
    if (pmTargetGroupRole == "dps")
    {
        if (groupRole != GroupRole::GroupRole_DPS)
        {
            todo = false;
        }
    }
    else if (pmTargetGroupRole == "healer")
    {
        if (groupRole != GroupRole::GroupRole_Healer)
        {
            todo = false;
        }
    }
    else if (pmTargetGroupRole == "tank")
    {
        if (groupRole != GroupRole::GroupRole_Tank)
        {
            todo = false;
        }
    }

    if (todo)
    {
        staying = true;
        if (me)
        {
            if (me->IsAlive())
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->AttackStop();
                me->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                sb->PetStop();
                sb->rm->ResetMovement();
            }
        }
        return true;
    }

    return false;
}

bool NingerStrategy::Hold(std::string pmTargetGroupRole)
{
    bool todo = true;
    if (pmTargetGroupRole == "dps")
    {
        if (groupRole != GroupRole::GroupRole_DPS)
        {
            todo = false;
        }
    }
    else if (pmTargetGroupRole == "healer")
    {
        if (groupRole != GroupRole::GroupRole_Healer)
        {
            todo = false;
        }
    }
    else if (pmTargetGroupRole == "tank")
    {
        if (groupRole != GroupRole::GroupRole_Tank)
        {
            todo = false;
        }
    }

    if (todo)
    {
        holding = true;
        staying = false;
        return true;
    }

    return false;
}

std::string NingerStrategy::GetGroupRoleName()
{
    if (!me)
    {
        return "";
    }
    switch (groupRole)
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

void NingerStrategy::SetGroupRole(std::string pmRoleName)
{
    if (!me)
    {
        return;
    }
    if (pmRoleName == "dps")
    {
        groupRole = GroupRole::GroupRole_DPS;
    }
    else if (pmRoleName == "tank")
    {
        groupRole = GroupRole::GroupRole_Tank;
    }
    else if (pmRoleName == "healer")
    {
        groupRole = GroupRole::GroupRole_Healer;
    }
}
*/
