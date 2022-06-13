#include "NingerAction_Base.h"
#include "World/World.h"
#include "Player.h"
#include "Pet.h"
#include "CreatureAI.h"
#include "Spell.h"
#include "TargetedMovementGenerator.h"
#include "GridNotifiers.h"

NingerMovement::NingerMovement(Player* pmMe)
{
    me = pmMe;
    ogTankTarget = ObjectGuid();
    ogChaseTarget = ObjectGuid();
    ogFollowTarget = ObjectGuid();
    positionTarget = me->GetPosition();
    distanceMax = 0.0f;
    distanceMin = 0.0f;
    holding = false;
    forceBack = false;
    moveCheckDelay = 0;
    activeMovementType = NingerMovementType::NingerMovementType_None;
}

void NingerMovement::ResetMovement()
{
    me->StopMoving();
    me->GetMotionMaster()->Clear();
    ogTankTarget = ObjectGuid();
    ogChaseTarget = ObjectGuid();
    ogFollowTarget = ObjectGuid();
    positionTarget = me->GetPosition();
    distanceMax = 0.0f;
    distanceMin = 0.0f;
    holding = false;
    forceBack = false;
    moveCheckDelay = 0;
    me->m_movementInfo.flags = 0;
    activeMovementType = NingerMovementType::NingerMovementType_None;
}

void NingerMovement::Run()
{
    if (me)
    {
        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
        {
            me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
        }
        if (me->IsWalking())
        {
            me->SetWalk(false);
        }
    }
}

void NingerMovement::Update_Direct(uint32 pmDiff)
{
    if (moveCheckDelay > 0)
    {
        moveCheckDelay -= pmDiff;
        return;
    }
    moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
    if (!me)
    {
        return;
    }
    if (!me->IsAlive())
    {
        return;
    }
    if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
    {
        return;
    }
    if (me->HasUnitState(UnitState::UNIT_STATE_NOT_MOVE))
    {
        return;
    }
    if (me->IsBeingTeleported())
    {
        return;
    }
    switch (activeMovementType)
    {
    case NingerMovementType::NingerMovementType_None:
    {
        break;
    }
    case NingerMovementType::NingerMovementType_Point:
    {
        float distance = me->GetExactDist(positionTarget);
        if (distance > VISIBILITY_DISTANCE_LARGE)
        {
            ResetMovement();
        }
        else
        {
            if (distance < MIN_DISTANCE_GAP)
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                activeMovementType = NingerMovementType::NingerMovementType_None;
            }
            else
            {
                if (!me->isMoving())
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, positionTarget);
                }
            }
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Tank:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogTankTarget))
        {
            if (holding)
            {
                if (me->isMoving())
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                {
                    ResetMovement();
                    break;
                }
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                if (targetDistance < chaseTarget->GetCombatReach() + me->GetCombatReach() + distanceMax)
                {
                    if (me->IsWithinLOSInMap(chaseTarget))
                    {
                        if (me->isMoving())
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                        }
                        if (!me->isInFront(chaseTarget, M_PI / 8))
                        {
                            me->SetFacingToObject(chaseTarget);
                        }
                        break;
                    }
                    else
                    {
                        chaseDistance = chaseDistance / 2.0f;
                    }
                }
                if (me->isMoving())
                {
                    bool ptValid = true;
                    if (chaseTarget->GetExactDist(positionTarget) > chaseTarget->GetCombatReach() + distanceMax)
                    {
                        ptValid = false;
                    }
                    else if (!chaseTarget->IsWithinLOS(positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ))
                    {
                        ptValid = false;
                    }
                    if (ptValid)
                    {
                        break;
                    }
                }
                Position predict = sNingerManager->PredictPosition(chaseTarget);
                chaseTarget->GetNearPoint(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, chaseDistance, chaseTarget->GetAngle(me), 0.0f, &predict);
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, positionTarget);
            }
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Chase:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogChaseTarget))
        {
            if (holding)
            {
                if (me->isMoving())
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                {
                    ResetMovement();
                    break;
                }
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                if (targetDistance < chaseTarget->GetCombatReach() + me->GetCombatReach() + distanceMax)
                {
                    bool minValid = true;
                    if (distanceMin > 0.0f)
                    {
                        if (targetDistance < chaseTarget->GetCombatReach() + distanceMin)
                        {
                            minValid = false;
                            chaseDistance = distanceMin + (distanceMax - distanceMin) / 2.0f;
                        }
                    }
                    if (minValid)
                    {
                        if (me->IsWithinLOSInMap(chaseTarget))
                        {
                            if (me->isMoving())
                            {
                                me->StopMoving();
                                me->GetMotionMaster()->Clear();
                            }
                            if (!me->isInFront(chaseTarget, M_PI / 8))
                            {
                                me->SetFacingToObject(chaseTarget);
                            }
                            break;
                        }
                        else
                        {
                            chaseDistance = chaseDistance / 2.0f;
                        }
                    }
                }
                if (me->isMoving())
                {
                    bool ptValid = true;
                    float ptDistance = chaseTarget->GetExactDist(positionTarget);
                    if (ptDistance > chaseTarget->GetCombatReach() + distanceMax)
                    {
                        ptValid = false;
                    }
                    else if (!chaseTarget->IsWithinLOS(positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ))
                    {
                        ptValid = false;
                    }
                    else if (distanceMin > 0.0f)
                    {
                        if (ptDistance < chaseTarget->GetCombatReach() + distanceMin)
                        {
                            ptValid = false;
                        }
                    }
                    if (ptValid)
                    {
                        break;
                    }
                }
                Position predict = sNingerManager->PredictPosition(chaseTarget);
                chaseTarget->GetNearPoint(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, chaseDistance, chaseTarget->GetAngle(me), 0.0f, &predict);
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, positionTarget);
            }
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Follow:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogFollowTarget))
        {
            if (holding)
            {
                if (me->isMoving())
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                {
                    ResetMovement();
                    break;
                }
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                if (targetDistance < chaseTarget->GetCombatReach() + me->GetCombatReach() + distanceMax)
                {
                    if (me->IsWithinLOSInMap(chaseTarget))
                    {
                        if (me->isMoving())
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                        }
                        if (!me->isInFront(chaseTarget, M_PI / 8))
                        {
                            me->SetFacingToObject(chaseTarget);
                        }
                        break;
                    }
                    else
                    {
                        chaseDistance = chaseDistance / 2.0f;
                    }
                }
                if (me->isMoving())
                {
                    bool ptValid = true;
                    float ptDistance = chaseTarget->GetExactDist(positionTarget);
                    if (ptDistance > chaseTarget->GetCombatReach() + distanceMax)
                    {
                        ptValid = false;
                    }
                    else if (!chaseTarget->IsWithinLOS(positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ))
                    {
                        ptValid = false;
                    }
                    else if (distanceMin > 0.0f)
                    {
                        if (ptDistance < chaseTarget->GetCombatReach() + distanceMin)
                        {
                            ptValid = false;
                        }
                    }
                    if (ptValid)
                    {
                        break;
                    }
                }
                Position predict = sNingerManager->PredictPosition(chaseTarget);
                float moveDistance = me->GetExactDist(predict);
                if (moveDistance > chaseDistance)
                {
                    moveDistance = moveDistance - chaseDistance;
                }
                if (moveDistance > chaseTarget->GetCombatReach())
                {
                    moveDistance = moveDistance - chaseTarget->GetCombatReach();
                }
                me->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, moveDistance, me->GetAngle(&predict));
                //chaseTarget->GetNearPoint(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, chaseDistance, chaseTarget->GetAngle(me), 0.0f, &predict);
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, positionTarget);
            }
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

void NingerMovement::Update_Motion(uint32 pmDiff)
{
    if (moveCheckDelay > 0)
    {
        moveCheckDelay -= pmDiff;
        return;
    }
    if (!me)
    {
        return;
    }
    if (!me->IsAlive())
    {
        return;
    }
    if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
    {
        return;
    }
    if (me->HasUnitState(UnitState::UNIT_STATE_NOT_MOVE))
    {
        return;
    }
    //if (me->HasUnitState(UnitState::UNIT_STATE_ROAMING_MOVE))
    //{
    //	return;
    //}
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return;
    }
    if (me->IsBeingTeleported())
    {
        return;
    }
    switch (activeMovementType)
    {
    case NingerMovementType::NingerMovementType_None:
    {
        break;
    }
    case NingerMovementType::NingerMovementType_Point:
    {
        float distance = me->GetDistance(positionTarget);
        if (distance > VISIBILITY_DISTANCE_LARGE)
        {
            ResetMovement();
        }
        else
        {
            if (distance < MIN_DISTANCE_GAP)
            {
                activeMovementType = NingerMovementType::NingerMovementType_None;
            }
            else
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::POINT_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, positionTarget);
                }
                moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
            }
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Tank:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogTankTarget))
        {
            if (holding)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::IDLE_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (me->IsWithinLOSInMap(chaseTarget) && targetDistance < chaseTarget->GetCombatReach() + chaseDistance)
                {
                    if (!me->isInFront(chaseTarget, M_PI / 8))
                    {
                        me->SetFacingToObject(chaseTarget);
                    }
                    return;
                }
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == MovementGeneratorType::CHASE_MOTION_TYPE)
                {
                    if (const ChaseMovementGenerator<Player>* mg = static_cast<ChaseMovementGenerator<Player> const*>(me->GetMotionMaster()->top()))
                    {
                        if (Unit* mgTarget = mg->GetTarget())
                        {
                            if (mgTarget->GetGUID() == chaseTarget->GetGUID())
                            {
                                break;
                            }
                        }
                    }
                }
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(chaseTarget, chaseDistance);
            }
            //moveCheckDelay = 100;
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Chase:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogChaseTarget))
        {
            if (holding)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::IDLE_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (me->IsWithinLOSInMap(chaseTarget) && targetDistance < chaseTarget->GetCombatReach() + chaseDistance)
                {
                    if (!me->isInFront(chaseTarget, M_PI / 8))
                    {
                        me->SetFacingToObject(chaseTarget);
                    }
                    return;
                }
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == MovementGeneratorType::CHASE_MOTION_TYPE)
                {
                    if (const ChaseMovementGenerator<Player>* mg = static_cast<ChaseMovementGenerator<Player> const*>(me->GetMotionMaster()->top()))
                    {
                        if (Unit* mgTarget = mg->GetTarget())
                        {
                            if (mgTarget->GetGUID() == chaseTarget->GetGUID())
                            {
                                if (distanceMin > 0.0f)
                                {
                                    if (targetDistance > chaseTarget->GetCombatReach() + distanceMin)
                                    {
                                        return;
                                    }
                                    else
                                    {
                                        chaseDistance = distanceMin + (distanceMax - distanceMin) / 2.0f;
                                    }
                                }
                                else
                                {
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
                float chaseAngle = 0.0f;
                if (forceBack)
                {
                    if (chaseTarget->GetTarget() != me->GetGUID())
                    {
                        chaseAngle = frand(M_PI - M_PI / 8, M_PI + M_PI / 8);
                    }
                }
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(chaseTarget, chaseDistance, chaseAngle);
            }
            moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Follow:
    {
        if (Unit* followTarget = ObjectAccessor::GetUnit(*me, ogFollowTarget))
        {
            if (holding)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::IDLE_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(followTarget, M_PI / 8))
                {
                    me->SetFacingToObject(followTarget);
                }
            }
            else
            {
                float followDistance = distanceMax;
                if (followDistance > MAX_DISTANCE_GAP)
                {
                    followDistance = followDistance - MAX_DISTANCE_GAP;
                }
                float targetDistance = me->GetExactDist(followTarget->GetPosition());
                if (me->IsWithinLOSInMap(followTarget) && targetDistance < followTarget->GetCombatReach() + followDistance)
                {
                    if (!me->isInFront(followTarget, M_PI / 8))
                    {
                        me->SetFacingToObject(followTarget);
                    }
                    return;
                }
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == MovementGeneratorType::CHASE_MOTION_TYPE)
                {
                    if (const ChaseMovementGenerator<Player>* mg = static_cast<ChaseMovementGenerator<Player> const*>(me->GetMotionMaster()->top()))
                    {
                        if (Unit* mgTarget = mg->GetTarget())
                        {
                            if (mgTarget->GetGUID() == followTarget->GetGUID())
                            {
                                if (me->isMoving())
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(followTarget, followDistance);
            }
            moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

void NingerMovement::Update_Follow(uint32 pmDiff)
{
    if (moveCheckDelay > 0)
    {
        moveCheckDelay -= pmDiff;
        return;
    }
    if (!me)
    {
        return;
    }
    if (!me->IsAlive())
    {
        return;
    }
    if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
    {
        return;
    }
    if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
    {
        return;
    }
    if (me->HasUnitState(UnitState::UNIT_STATE_NOT_MOVE))
    {
        return;
    }
    //if (me->HasUnitState(UnitState::UNIT_STATE_ROAMING_MOVE))
    //{
    //	return;
    //}
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return;
    }
    if (me->IsBeingTeleported())
    {
        return;
    }
    switch (activeMovementType)
    {
    case NingerMovementType::NingerMovementType_None:
    {
        break;
    }
    case NingerMovementType::NingerMovementType_Point:
    {
        float distance = me->GetDistance(positionTarget);
        if (distance > VISIBILITY_DISTANCE_LARGE)
        {
            ResetMovement();
        }
        else
        {
            if (distance < MIN_DISTANCE_GAP)
            {
                activeMovementType = NingerMovementType::NingerMovementType_None;
            }
            else
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::POINT_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, positionTarget);
                }
                moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
            }
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Tank:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogTankTarget))
        {
            if (holding)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::IDLE_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == MovementGeneratorType::FOLLOW_MOTION_TYPE)
                {
                    if (const FollowMovementGenerator<Player>* mg = static_cast<FollowMovementGenerator<Player> const*>(me->GetMotionMaster()->top()))
                    {
                        if (Unit* mgTarget = mg->GetTarget())
                        {
                            if (mgTarget->GetGUID() == chaseTarget->GetGUID())
                            {
                                if (me->isMoving())
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (targetDistance < chaseTarget->GetCombatReach() + chaseDistance)
                {
                    if (me->IsWithinLOSInMap(chaseTarget))
                    {
                        if (me->isMoving())
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                        }
                        if (!me->isInFront(chaseTarget, M_PI / 8))
                        {
                            me->SetFacingToObject(chaseTarget);
                        }
                        return;
                    }
                    else
                    {
                        chaseDistance = chaseDistance / 2.0f;
                    }
                }
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveFollow(chaseTarget, chaseDistance, 0.0f);
            }
            //moveCheckDelay = 100;
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Chase:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogChaseTarget))
        {
            if (holding)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::IDLE_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(chaseTarget, M_PI / 8))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == MovementGeneratorType::FOLLOW_MOTION_TYPE)
                {
                    if (const FollowMovementGenerator<Player>* mg = static_cast<FollowMovementGenerator<Player> const*>(me->GetMotionMaster()->top()))
                    {
                        if (Unit* mgTarget = mg->GetTarget())
                        {
                            if (mgTarget->GetGUID() == chaseTarget->GetGUID())
                            {
                                if (me->isMoving())
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                float chaseDistance = distanceMax;
                if (chaseDistance > MAX_DISTANCE_GAP)
                {
                    chaseDistance = chaseDistance - MAX_DISTANCE_GAP;
                }
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (targetDistance < chaseTarget->GetCombatReach() + chaseDistance)
                {
                    bool minValid = true;
                    if (distanceMin > 0.0f)
                    {
                        if (targetDistance < chaseTarget->GetCombatReach() + distanceMin)
                        {
                            minValid = false;
                            chaseDistance = distanceMin + (chaseDistance - distanceMin) / 2.0f;
                        }
                    }
                    if (minValid)
                    {
                        if (me->IsWithinLOSInMap(chaseTarget))
                        {
                            if (me->isMoving())
                            {
                                me->StopMoving();
                                me->GetMotionMaster()->Clear();
                            }
                            if (!me->isInFront(chaseTarget, M_PI / 8))
                            {
                                me->SetFacingToObject(chaseTarget);
                            }
                            return;
                        }
                        else
                        {
                            chaseDistance = chaseDistance / 2.0f;
                        }
                    }
                }
                float chaseAngle = 0.0f;
                if (forceBack)
                {
                    if (chaseTarget->GetTarget() != me->GetGUID())
                    {
                        chaseAngle = frand(M_PI - M_PI / 8, M_PI + M_PI / 8);
                    }
                }
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveFollow(chaseTarget, chaseDistance, chaseAngle);
            }
            moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Follow:
    {
        if (Unit* followTarget = ObjectAccessor::GetUnit(*me, ogFollowTarget))
        {
            if (holding)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::IDLE_MOTION_TYPE)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                }
                if (!me->isInFront(followTarget, M_PI / 8))
                {
                    me->SetFacingToObject(followTarget);
                }
            }
            else
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == MovementGeneratorType::FOLLOW_MOTION_TYPE)
                {
                    if (const FollowMovementGenerator<Player>* mg = static_cast<FollowMovementGenerator<Player> const*>(me->GetMotionMaster()->top()))
                    {
                        if (Unit* mgTarget = mg->GetTarget())
                        {
                            if (mgTarget->GetGUID() == followTarget->GetGUID())
                            {
                                if (me->isMoving())
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                float followDistance = distanceMax;
                if (followDistance > MAX_DISTANCE_GAP)
                {
                    followDistance = followDistance - MAX_DISTANCE_GAP;
                }
                float targetDistance = me->GetExactDist(followTarget->GetPosition());
                if (targetDistance < followTarget->GetCombatReach() + followDistance)
                {
                    if (me->IsWithinLOSInMap(followTarget))
                    {
                        if (me->isMoving())
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                        }
                        if (!me->isInFront(followTarget, M_PI / 8))
                        {
                            me->SetFacingToObject(followTarget);
                        }
                        return;
                    }
                    else
                    {
                        followDistance = followDistance / 2.0f;
                    }
                }
                Run();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveFollow(followTarget, distanceMax, 0.0f);

            }
            moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
        }
        else
        {
            ResetMovement();
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

bool NingerMovement::Tank(Unit* pmTankTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    distanceMax = pmDistanceMax;
    distanceMin = pmDistanceMin;
    holding = pmHolding;
    if (me && pmTankTarget)
    {
        float distance = me->GetDistance(pmTankTarget);
        if (distance < VISIBILITY_DISTANCE_NORMAL)
        {
            activeMovementType = NingerMovementType::NingerMovementType_Tank;
            if (ogTankTarget != pmTankTarget->GetGUID())
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                ogTankTarget = pmTankTarget->GetGUID();
                moveCheckDelay = 0;
            }
            return true;
        }
    }

    return false;
}

bool NingerMovement::Chase(Unit* pmChaseTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmForceBack)
{
    if (me && pmChaseTarget)
    {
        if (me->IsWithinDist(pmChaseTarget, VISIBILITY_DISTANCE_NORMAL))
        {
            distanceMax = pmDistanceMax;
            distanceMin = pmDistanceMin;
            holding = pmHolding;
            forceBack = pmForceBack;
            activeMovementType = NingerMovementType::NingerMovementType_Chase;
            if (ogChaseTarget != pmChaseTarget->GetGUID())
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                ogChaseTarget = pmChaseTarget->GetGUID();
                moveCheckDelay = 0;
            }
            return true;
        }
    }

    return false;
}

bool NingerMovement::Follow(Unit* pmFollowTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    if (me && pmFollowTarget)
    {
        if (me->IsWithinDist(pmFollowTarget, VISIBILITY_DISTANCE_NORMAL))
        {
            distanceMax = pmDistanceMax;
            distanceMin = pmDistanceMin;
            holding = pmHolding;
            activeMovementType = NingerMovementType::NingerMovementType_Follow;
            if (ogFollowTarget != pmFollowTarget->GetGUID())
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                ogFollowTarget = pmFollowTarget->GetGUID();
                moveCheckDelay = 0;
            }
            return true;
        }
    }

    return false;
}

void NingerMovement::Point(Position pmPosTarget, uint32 pmLimit)
{
    if (!me)
    {
        return;
    }
    float distance = me->GetDistance(pmPosTarget.m_positionX, pmPosTarget.m_positionY, pmPosTarget.m_positionZ);
    if (distance < VISIBILITY_DISTANCE_NORMAL)
    {
        activeMovementType = NingerMovementType::NingerMovementType_Point;
        if (me->IsNonMeleeSpellCast(false))
        {
            me->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
            me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
        }
        positionTarget = pmPosTarget;

        Run();
        me->StopMoving();
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MovePoint(0, positionTarget);
        moveCheckDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
    }
}

Position NingerMovement::GetDestPosition(Unit* pmTarget, float pmMoveDistance, bool pmCloser)
{
    Position result;

    bool found = false;
    if (pmMoveDistance < VISIBILITY_DISTANCE_NORMAL)
    {
        float moveAngle = me->GetAngle(pmTarget);
        if (!pmCloser)
        {
            moveAngle += M_PI;
        }
        float checkDistance = pmMoveDistance;
        while (checkDistance < RANGE_MAX_DISTANCE)
        {
            me->GetNearPoint(me, result.m_positionX, result.m_positionY, result.m_positionZ, 0.0f, checkDistance + MIN_DISTANCE_GAP, moveAngle);
            me->UpdateGroundPositionZ(result.m_positionX, result.m_positionY, result.m_positionZ);
            if (pmTarget->IsWithinLOS(result.m_positionX, result.m_positionY, result.m_positionZ))
            {
                found = true;
                break;
            }
            checkDistance += MAX_DISTANCE_GAP;
        }
    }
    if (!found)
    {
        result = pmTarget->GetPosition();
    }

    return result;
}

bool NingerMovement::Direction(Unit* pmCommander, uint32 pmDirection, uint32 pmLimit, float pmDistance)
{
    bool result = false;

    if (pmCommander)
    {
        float absAngle = pmCommander->GetOrientation();
        switch (pmDirection)
        {
        case NingerMovementDirection_Forward:
        {
            break;
        }
        case NingerMovementDirection_Back:
        {
            absAngle = absAngle + M_PI;
            break;
        }
        case NingerMovementDirection_Left:
        {
            absAngle = absAngle + M_PI / 2;
            break;
        }
        case NingerMovementDirection_Right:
        {
            absAngle = absAngle + M_PI * 3 / 2;
            break;
        }
        default:
        {
            break;
        }
        }
        me->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, me->GetObjectSize(), pmDistance, absAngle);
        activeMovementType = NingerMovementType::NingerMovementType_Point;
        if (me->IsNonMeleeSpellCast(false))
        {
            me->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
            me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
        }
        Run();
        me->StopMoving();
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MovePoint(0, positionTarget);
        result = true;
    }

    return result;
}

bool NingerMovement::Direction(float pmAngle, uint32 pmLimit, float pmDistance)
{
    bool result = false;

    me->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, me->GetObjectSize(), pmDistance, pmAngle);
    activeMovementType = NingerMovementType::NingerMovementType_Point;
    Run();
    if (me->IsNonMeleeSpellCast(false))
    {
        me->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
        me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
    }
    me->StopMoving();
    me->GetMotionMaster()->Clear();
    me->GetMotionMaster()->MovePoint(0, positionTarget);
    result = true;

    return result;
}

NingerAction_Base::NingerAction_Base(Player* pmMe)
{
    me = pmMe;
    nm = new NingerMovement(me);
    specialty = 0;
}

void NingerAction_Base::Prepare()
{
    if (me)
    {
        me->SetPvP(true);
        me->DurabilityRepairAll(false, 0, false);
        if (!me->GetGroup())
        {
            if (me->GetMap()->Instanceable())
            {
                me->TeleportTo(me->m_homebindMapId, me->m_homebindX, me->m_homebindY, me->m_homebindZ, 0);
            }
        }
    }
}

void NingerAction_Base::Reset()
{
    ClearTarget();
}

void NingerAction_Base::Update(uint32 pmDiff)
{
    nm->Update_Direct(pmDiff);
}

bool NingerAction_Base::DPS(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack)
{
    return false;
}

bool NingerAction_Base::AOE(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing)
{
    return false;
}

bool NingerAction_Base::Tank(Unit* pmTarget, bool pmAOE, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    return false;
}

bool NingerAction_Base::Follow(Unit* pmFollowTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return true;
    }
    if (!pmFollowTarget)
    {
        return false;
    }
    if (!nm->Follow(pmFollowTarget, pmDistanceMax, pmDistanceMin, pmHolding))
    {
        if (me->GetTarget() == pmFollowTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    ChooseTarget(pmFollowTarget);
}

bool NingerAction_Base::Heal(Unit* pmTarget, bool pmInstantOnly)
{
    return false;
}

bool NingerAction_Base::ReadyTank(Unit* pmTarget)
{
    return false;
}

bool NingerAction_Base::GroupHeal(Unit* pmTarget, bool pmInstantOnly)
{
    return false;
}

bool NingerAction_Base::SimpleHeal(Unit* pmTarget, bool pmInstantOnly)
{
    return false;
}

bool NingerAction_Base::Cure(Unit* pmTarget)
{
    return false;
}

bool NingerAction_Base::Buff(Unit* pmTarget)
{
    return false;
}

uint32 NingerAction_Base::Caution()
{
    if (me)
    {
        if (me->IsAlive())
        {
            float startAngle = me->GetOrientation() + M_PI / 4;
            float angleGap = 0.0f;
            Position pos;
            while (angleGap < M_PI * 2)
            {
                me->GetNearPoint(me, pos.m_positionX, pos.m_positionY, pos.m_positionZ, me->GetObjectSize(), RANGE_NEAR_DISTANCE + MAX_DISTANCE_GAP, startAngle + angleGap);
                if (me->GetDistance(pos.m_positionX, pos.m_positionY, pos.m_positionZ) > RANGE_NEAR_DISTANCE - MIN_DISTANCE_GAP)
                {
                    me->InterruptNonMeleeSpells(false);
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, pos);
                    return 2000;
                }
            }
        }
    }

    return 0;
}

bool NingerAction_Base::Mark(Unit* pmTarget, int pmRTI)
{
    return false;
}

bool NingerAction_Base::Assist(int pmRTI)
{
    return false;
}

bool NingerAction_Base::Revive(Player* pmTarget)
{
    return false;
}

bool NingerAction_Base::Petting(bool pmSummon, bool pmReset)
{
    return false;
}

void NingerAction_Base::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
{

}

void NingerAction_Base::ResetTalent()
{

}

bool NingerAction_Base::InitializeEquipments(bool pmReset)
{
    return true;
}

void NingerAction_Base::RemoveEquipments()
{
    if (!me)
    {
        return;
    }
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        if (Item* inventoryItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            me->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
        }
    }
    for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
    {
        if (Item* currentEquip = me->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
        {
            me->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
        }
    }
}

void NingerAction_Base::LearnTalent(uint32 pmTalentId, uint32 pmMaxRank)
{
    if (!me)
    {
        return;
    }
    uint32 checkRank = 0;
    while (checkRank < pmMaxRank)
    {
        me->LearnTalent(pmTalentId, checkRank);
        checkRank++;
    }
}

void NingerAction_Base::TrainSpells(uint32 pmTrainerEntry)
{
    TrainerSpellData const* trainer_spells = sObjectMgr->GetNpcTrainerSpells(pmTrainerEntry);
    bool hasNew = false;
    while (true)
    {
        hasNew = false;
        for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
        {
            TrainerSpell const* tSpell = &itr->second;
            if (me->HasSpell(tSpell->spell))
            {
                continue;
            }
            if (tSpell->reqSpell && !me->HasSpell(tSpell->reqSpell))
            {
                continue;
            }
            TrainerSpellState state = me->GetTrainerSpellState(tSpell);
            if (state == TrainerSpellState::TRAINER_SPELL_GREEN)
            {
                me->learnSpell(tSpell->spell);
                hasNew = true;
            }
        }
        if (!hasNew)
        {
            break;
        }
    }
}

void NingerAction_Base::EquipRandomItem(uint32 pmEquipSlot, uint32 pmClass, uint32 pmSubclass, uint32 pmMinQuality, int pmModType, std::unordered_set<uint32> pmInventoryTypeSet)
{
    bool checkStat = true;
    if (pmModType < 0)
    {
        checkStat = false;
    }
    uint32 inventoryType = 0;
    if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
    {
        inventoryType = 1;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
    {
        inventoryType = 3;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS)
    {
        inventoryType = 9;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST)
    {
        inventoryType = 6;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET)
    {
        inventoryType = 8;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
    {
        inventoryType = 10;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST)
    {
        inventoryType = 5;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS)
    {
        inventoryType = 7;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_BACK)
    {
        inventoryType = 16;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
    {
        inventoryType = 2;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1)
    {
        inventoryType = 11;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
    {
        inventoryType = 11;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_MAINHAND)
    {
        inventoryType = 17;
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_OFFHAND)
    {

    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
    {
        inventoryType = 15;
    }
    else
    {
        return;
    }

    std::ostringstream inventoryTypeQueryStream;
    if (pmInventoryTypeSet.size() > 0)
    {
        inventoryTypeQueryStream << " ( 1 = 0 ";
        for (std::unordered_set<uint32>::iterator it = pmInventoryTypeSet.begin(); it != pmInventoryTypeSet.end(); it++)
        {
            inventoryTypeQueryStream << " or InventoryType = " << *it;
        }
        inventoryTypeQueryStream << " ) ";
    }
    else
    {
        inventoryTypeQueryStream << " InventoryType = " << inventoryType;
    }
    std::string inventoryTypeQuery = inventoryTypeQueryStream.str();
    int activeReqLevel = me->getLevel();
    int minReqLevel = activeReqLevel - 3;
    while (minReqLevel > 0)
    {
        int activeQuality = ItemQualities::ITEM_QUALITY_EPIC;
        while (activeQuality >= pmMinQuality)
        {
            std::unordered_set<uint32> availableItemEntrySet;
            std::ostringstream queryStream;
            queryStream << "SELECT entry FROM item_template where " << inventoryTypeQuery << " and class = " << pmClass << " and subclass = " << pmSubclass << " and Quality = " << activeQuality << " and RequiredLevel >= " << minReqLevel << " and RequiredLevel <= " << activeReqLevel << " and AllowableClass = -1 and AllowableRace = -1 and RequiredSkill = 0 and requiredspell = 0 and requiredhonorrank = 0 and RequiredCityRank = 0 and RequiredReputationFaction = 0 order by rand()";
            QueryResult itemQR = WorldDatabase.Query(queryStream.str().c_str());
            if (itemQR)
            {
                do
                {
                    Field* fields = itemQR->Fetch();
                    availableItemEntrySet.insert(fields[0].Get<uint32>());
                } while (itemQR->NextRow());
            }
            for (std::unordered_set<uint32>::iterator entryIt = availableItemEntrySet.begin(); entryIt != availableItemEntrySet.end(); entryIt++)
            {
                if (const ItemTemplate* pProto = sObjectMgr->GetItemTemplate(*entryIt))
                {
                    bool hasStat = false;
                    if (checkStat)
                    {
                        if (pProto->RandomProperty > 0 || pProto->RandomSuffix > 0)
                        {
                            hasStat = true;
                        }
                        else
                        {
                            for (uint32 statIndex = 0; statIndex < pProto->StatsCount; statIndex++)
                            {
                                if (pProto->ItemStat[statIndex].ItemStatType == pmModType)
                                {
                                    hasStat = true;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        hasStat = true;
                    }
                    if (hasStat)
                    {
                        if (Item* pItem = Item::CreateItem(*entryIt, 1))
                        {
                            uint16 dest = 0;
                            if (me->CanEquipItem(pmEquipSlot, dest, pItem, false) == InventoryResult::EQUIP_ERR_OK)
                            {
                                me->EquipItem(dest, pItem, true);
                                std::ostringstream msgStream;
                                msgStream << me->GetName() << " Equiped " << pItem->GetTemplate()->Name1;
                                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
                                sLog->outMessage(NINGER_MARK, LogLevel::LOG_LEVEL_DEBUG, msgStream.str().c_str());
                                return;
                            }
                        }
                    }
                }
            }
            activeQuality--;
        }
        activeReqLevel -= 4;
        minReqLevel = activeReqLevel - 3;
    }
}

void NingerAction_Base::PetAttack(Unit* pmTarget)
{
    if (me)
    {
        if (Pet* myPet = me->GetPet())
        {
            if (myPet->IsAlive())
            {
                if (CreatureAI* cai = myPet->AI())
                {
                    cai->AttackStart(pmTarget);
                }
            }
        }
    }
}

void NingerAction_Base::PetStop()
{
    if (me)
    {
        if (Pet* myPet = me->GetPet())
        {
            myPet->AttackStop();
            if (CharmInfo* pci = myPet->GetCharmInfo())
            {
                if (pci->IsCommandAttack())
                {
                    pci->SetIsCommandAttack(false);
                }
                if (!pci->IsCommandFollow())
                {
                    pci->SetIsCommandFollow(true);
                }
            }
        }
    }
}

bool NingerAction_Base::UseItem(Item* pmItem, Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    if (me->CanUseItem(pmItem) != EQUIP_ERR_OK)
    {
        return false;
    }

    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return false;
    }

    if (const ItemTemplate* proto = pmItem->GetTemplate())
    {
        ChooseTarget(pmTarget);
        SpellCastTargets targets;
        targets.Update(pmTarget);
        me->CastItemUseSpell(pmItem, targets, 1, 0);
        return true;
    }

    return false;
}

bool NingerAction_Base::UseItem(Item* pmItem, Item* pmTarget)
{
    if (!me)
    {
        return false;
    }
    if (me->CanUseItem(pmItem) != EQUIP_ERR_OK)
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return false;
    }
    if (!pmTarget)
    {
        return false;
    }

    if (const ItemTemplate* proto = pmItem->GetTemplate())
    {
        SpellCastTargets targets;
        targets.SetItemTarget(pmTarget);
        me->CastItemUseSpell(pmItem, targets, 1, 0);
        return true;
    }

    return false;
}

bool NingerAction_Base::CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura, bool pmOnlyMyAura, bool pmClearShapeShift, uint32 pmMaxAuraStack)
{
    if (!SpellValid(pmSpellId))
    {
        return false;
    }
    if (!me)
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return true;
    }
    if (pmClearShapeShift)
    {
        me->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    }
    if (const SpellInfo* pS = sSpellMgr->GetSpellInfo(pmSpellId))
    {
        if (pmTarget)
        {
            if (!me->IsWithinLOSInMap(pmTarget))
            {
                return false;
            }
            if (pmTarget->IsImmunedToSpell(pS))
            {
                return false;
            }
            if (pmCheckAura)
            {
                Aura* targetAura = nullptr;
                if (pmOnlyMyAura)
                {
                    targetAura = pmTarget->GetAura(pmSpellId, me->GetGUID());
                }
                else
                {
                    targetAura = pmTarget->GetAura(pmSpellId);
                }
                if (targetAura)
                {
                    if (targetAura->GetStackAmount() >= pmMaxAuraStack)
                    {
                        return false;
                    }
                }
            }
            if (!me->isInFront(pmTarget, M_PI / 4))
            {
                me->SetFacingToObject(pmTarget);
            }
            if (me->GetTarget() != pmTarget->GetGUID())
            {
                ChooseTarget(pmTarget);
            }
        }
        for (size_t i = 0; i < MAX_SPELL_REAGENTS; i++)
        {
            if (pS->Reagent[i] > 0)
            {
                if (!me->HasItemCount(pS->Reagent[i], pS->ReagentCount[i]))
                {
                    me->StoreNewItemInBestSlots(pS->Reagent[i], pS->ReagentCount[i] * 10);
                }
            }
        }
        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
        }
        //me->CastSpell(pmTarget, pS, TriggerCastFlags::TRIGGERED_NONE);
        //return true;

        SpellCastResult scr = me->CastSpell(pmTarget, pS, TriggerCastFlags::TRIGGERED_NONE);
        if (scr == SpellCastResult::SPELL_CAST_OK)
        {
            return true;
        }
    }

    return false;
}

void NingerAction_Base::CancelAura(uint32 pmSpellID)
{
    if (pmSpellID == 0)
    {
        return;
    }
    if (!me)
    {
        return;
    }
    me->RemoveAurasDueToSpell(pmSpellID);
}

bool NingerAction_Base::Eat()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (me->IsInCombat())
    {
        return false;
    }
    uint32 foodEntry = 0;
    uint32 myLevel = me->getLevel();
    if (myLevel >= 75)
    {
        foodEntry = 35950;
    }
    else if (myLevel >= 65)
    {
        foodEntry = 33449;
    }
    else if (myLevel >= 55)
    {
        foodEntry = 27855;
    }
    else if (myLevel >= 45)
    {
        foodEntry = 8950;
    }
    else if (myLevel >= 35)
    {
        foodEntry = 4601;
    }
    else if (myLevel >= 25)
    {
        foodEntry = 4544;
    }
    else if (myLevel >= 15)
    {
        foodEntry = 4542;
    }
    else if (myLevel >= 5)
    {
        foodEntry = 4541;
    }
    else
    {
        foodEntry = 4540;
    }
    if (!me->HasItemCount(foodEntry, 1))
    {
        me->StoreNewItemInBestSlots(foodEntry, 20);
    }
    me->CombatStop(true);
    me->StopMoving();
    me->GetMotionMaster()->Clear();
    ClearTarget();

    Item* pFood = GetItemInInventory(foodEntry);
    if (pFood && !pFood->IsInTrade())
    {
        if (UseItem(pFood, me))
        {
            nm->ResetMovement();
            return true;
        }
    }
    return false;
}

bool NingerAction_Base::Drink()
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (me->IsInCombat())
    {
        return false;
    }
    uint32 drinkEntry = 0;
    uint32 myLevel = me->getLevel();
    if (myLevel >= 75)
    {
        drinkEntry = 33445;
    }
    else if (myLevel >= 70)
    {
        drinkEntry = 33444;
    }
    else if (myLevel >= 65)
    {
        drinkEntry = 27860;
    }
    else if (myLevel >= 60)
    {
        drinkEntry = 28399;
    }
    else if (myLevel >= 55)
    {
        drinkEntry = 18300;
    }
    else if (myLevel >= 45)
    {
        drinkEntry = 8766;
    }
    else if (myLevel >= 35)
    {
        drinkEntry = 1645;
    }
    else if (myLevel >= 25)
    {
        drinkEntry = 1708;
    }
    else if (myLevel >= 15)
    {
        drinkEntry = 1205;
    }
    else if (myLevel >= 5)
    {
        drinkEntry = 1179;
    }
    else
    {
        drinkEntry = 159;
    }

    if (!me->HasItemCount(drinkEntry, 1))
    {
        me->StoreNewItemInBestSlots(drinkEntry, 20);
    }
    me->CombatStop(true);
    me->StopMoving();
    me->GetMotionMaster()->Clear();
    ClearTarget();
    Item* pDrink = GetItemInInventory(drinkEntry);
    if (pDrink && !pDrink->IsInTrade())
    {
        if (UseItem(pDrink, me))
        {
            nm->ResetMovement();
            return true;
        }
    }

    return false;
}

bool NingerAction_Base::HealthPotion()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (!me->IsInCombat())
    {
        return false;
    }
    uint32 potionEntry = 0;
    uint32 myLevel = me->getLevel();
    if (myLevel >= 70)
    {
        potionEntry = 33447;
    }
    else if (myLevel >= 55)
    {
        potionEntry = 22829;
    }
    else if (myLevel >= 45)
    {
        potionEntry = 13446;
    }
    else if (myLevel >= 35)
    {
        potionEntry = 3928;
    }
    else if (myLevel >= 21)
    {
        potionEntry = 1710;
    }
    else if (myLevel >= 12)
    {
        potionEntry = 929;
    }
    else if (myLevel >= 3)
    {
        potionEntry = 858;
    }
    else
    {
        potionEntry = 118;
    }
    if (!me->HasItemCount(potionEntry, 1))
    {
        me->StoreNewItemInBestSlots(potionEntry, 20);
    }
    Item* pPotion = GetItemInInventory(potionEntry);
    if (pPotion && !pPotion->IsInTrade())
    {
        if (UseItem(pPotion, me))
        {
            return true;
        }
    }
    return false;
}

bool NingerAction_Base::ManaPotion()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (!me->IsInCombat())
    {
        return false;
    }
    uint32 potionEntry = 0;
    uint32 myLevel = me->getLevel();
    if (myLevel >= 70)
    {
        potionEntry = 33448;
    }
    else if (myLevel >= 55)
    {
        potionEntry = 22832;
    }
    else if (myLevel >= 49)
    {
        potionEntry = 13444;
    }
    else if (myLevel >= 41)
    {
        potionEntry = 13443;
    }
    else if (myLevel >= 31)
    {
        potionEntry = 6149;
    }
    else if (myLevel >= 22)
    {
        potionEntry = 3827;
    }
    else if (myLevel >= 14)
    {
        potionEntry = 3385;
    }
    else if (myLevel >= 5)
    {
        potionEntry = 2455;
    }
    if (potionEntry > 0)
    {
        if (!me->HasItemCount(potionEntry, 1))
        {
            me->StoreNewItemInBestSlots(potionEntry, 20);
        }
        Item* pPotion = GetItemInInventory(potionEntry);
        if (pPotion && !pPotion->IsInTrade())
        {
            if (UseItem(pPotion, me))
            {
                return true;
            }
        }
    }

    return false;
}

bool NingerAction_Base::RandomTeleport()
{
    int myLevel = me->getLevel();
    std::unordered_map<uint32, ObjectGuid> samePlayersMap;
    std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
    for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
    {
        if (WorldSession* eachWS = wsIT->second)
        {
            if (!eachWS->isNinger)
            {
                if (Player* eachPlayer = eachWS->GetPlayer())
                {
                    int eachLevel = eachPlayer->getLevel();
                    if (myLevel > eachLevel - 5 && myLevel < eachLevel + 5)
                    {
                        //if (me->IsHostileTo(eachPlayer))
                        //{
                        //    samePlayersMap[samePlayersMap.size()] = eachPlayer->GetGUID();
                        //}
                        samePlayersMap[samePlayersMap.size()] = eachPlayer->GetGUID();
                    }
                }
            }
        }
    }
    uint32 targetPlayerIndex = urand(0, samePlayersMap.size());
    if (Player* targetPlayer = ObjectAccessor::FindPlayer(samePlayersMap[targetPlayerIndex]))
    {
        float nearX = 0.0f;
        float nearY = 0.0f;
        float nearZ = 0.0f;
        float nearDistance = frand(200.0f, 400.0f);
        float nearAngle = frand(0.0f, M_PI * 2);
        targetPlayer->GetNearPoint(targetPlayer, nearX, nearY, nearZ, targetPlayer->GetCombatReach(), nearDistance, nearAngle);
        me->TeleportTo(me->GetMapId(), nearX, nearY, nearZ, 0);
    }
    return false;
}

void NingerAction_Base::ChooseTarget(Unit* pmTarget)
{
    if (pmTarget)
    {
        if (me)
        {
            me->SetSelection(pmTarget->GetGUID());
            me->SetTarget(pmTarget->GetGUID());
        }
    }
}

void NingerAction_Base::ClearTarget()
{
    if (me)
    {
        me->SetSelection(ObjectGuid::Empty);
        me->SetTarget(ObjectGuid::Empty);
        me->AttackStop();
        me->InterruptNonMeleeSpells(true);
    }
}

bool NingerAction_Base::SpellValid(uint32 pmSpellID)
{
    if (pmSpellID == 0)
    {
        return false;
    }
    if (!me)
    {
        return false;
    }
    if (me->HasSpellCooldown(pmSpellID))
    {
        return false;
    }

    return true;
}

Item* NingerAction_Base::GetItemInInventory(uint32 pmEntry)
{
    if (!me)
    {
        return NULL;
    }
    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item* pItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pItem)
        {
            if (pItem->GetEntry() == pmEntry)
            {
                return pItem;
            }
        }
    }

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        if (Bag* pBag = (Bag*)me->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            for (uint32 j = 0; j < pBag->GetBagSize(); j++)
            {
                Item* pItem = me->GetItemByPos(i, j);
                if (pItem)
                {
                    if (pItem->GetEntry() == pmEntry)
                    {
                        return pItem;
                    }
                }
            }
        }
    }

    return NULL;
}

Player* NingerAction_Base::GetNearbyHostilePlayer(float pmRange)
{
    std::list<Player*> players;
    Acore::AnyPlayerInObjectRangeCheck checker(me, pmRange);
    Acore::PlayerListSearcher<Acore::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
    Cell::VisitWorldObjects(me, searcher, pmRange);
    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if (Player* eachPlayer = *itr)
        {
            if (eachPlayer->IsAlive())
            {
                if (me->IsValidAttackTarget(eachPlayer))
                {
                    if (eachPlayer->CanSeeOrDetect(me))
                    {
                        return eachPlayer;
                    }
                }
            }
        }
    }

    return nullptr;
}

Unit* NingerAction_Base::GetNearbyHostileUnit(float pmRange)
{
    std::list<Creature*> creatureList;
    me->GetCreatureListWithEntryInGrid(creatureList, 0, pmRange);
    if (!creatureList.empty())
    {
        for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
        {
            if (Creature* hostileCreature = *itr)
            {
                if (hostileCreature->IsAlive())
                {
                    if (!hostileCreature->IsCivilian())
                    {
                        if (me->IsValidAttackTarget(hostileCreature))
                        {
                            return hostileCreature;
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}
