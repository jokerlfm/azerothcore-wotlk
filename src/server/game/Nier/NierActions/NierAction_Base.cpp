#include "NierAction_Base.h"
#include "World/World.h"
#include "Player.h"
#include "Pet.h"
#include "CreatureAI.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "Map.h"
#include "SpellMgr.h"
#include "SpellAuras.h"
#include "Bag.h"
#include "TargetedMovementGenerator.h"

NierMovement::NierMovement(Player* pmMe)
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
    backwardCheckDelay = 0;
    circleCheckDelay = 0;
    activeMovementType = NierMovementType::NierMovementType_None;
}

void NierMovement::ResetMovement()
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
    activeMovementType = NierMovementType::NierMovementType_None;
}

void NierMovement::Run()
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

void NierMovement::Update_Direct(uint32 pmDiff)
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
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return;
    }
    switch (activeMovementType)
    {
    case NierMovementType::NierMovementType_None:
    {
        break;
    }
    case NierMovementType::NierMovementType_Point:
    {
        float distance = me->GetExactDist(positionTarget);
        if (distance > VISIBILITY_DISTANCE_LARGE)
        {
            ResetMovement();
        }
        else
        {
            if (distance < CONTACT_DISTANCE)
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                activeMovementType = NierMovementType::NierMovementType_None;
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
    case NierMovementType::NierMovementType_Tank:
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
                if (!me->isInFront(chaseTarget, M_PI / 2))
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
                if (chaseDistance > MELEE_RANGE)
                {
                    chaseDistance = chaseDistance - MELEE_RANGE;
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
                        if (!me->isInFront(chaseTarget, M_PI / 2))
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
                //Position predict = sNierManager->PredictPosition(chaseTarget);
                chaseTarget->GetNearPoint(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, chaseDistance, chaseTarget->GetAbsoluteAngle(me->GetPosition()));
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
    case NierMovementType::NierMovementType_Chase:
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
                if (!me->isInFront(chaseTarget, M_PI / 2))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (backwardCheckDelay < 0)
                {
                    if (targetDistance < me->GetMeleeRange(chaseTarget) / 4.0f)
                    {
                        float moveDist = me->GetMeleeRange(chaseTarget) / 2.0f;
                        me->GetMotionMaster()->MoveBackwards(chaseTarget, moveDist);
                        break;
                    }

                    backwardCheckDelay = 20;
                }
                else
                {
                    backwardCheckDelay -= 1;
                }
                if (circleCheckDelay < 0)
                {
                    me->GetMotionMaster()->MoveCircleTarget(chaseTarget);
                    circleCheckDelay = 20;
                }
                else
                {
                    circleCheckDelay -= 1;
                }
                if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                {
                    ResetMovement();
                    break;
                }
                float chaseDistance = distanceMax;
                if (targetDistance < (chaseTarget->GetCombatReach() + me->GetCombatReach()) / 2.0f + distanceMax)
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
                            if (!me->isInFront(chaseTarget, M_PI / 2))
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
                    if (ptDistance > (chaseTarget->GetCombatReach() + me->GetCombatReach()) / 2.0f + distanceMax)
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
                //Position predict = sNierManager->PredictPosition(chaseTarget);
                //chaseTarget->GetNearPoint(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, chaseDistance, chaseTarget->GetAbsoluteAngle(me->GetPosition()));
                positionTarget = chaseTarget->GetPosition();
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
    case NierMovementType::NierMovementType_Follow:
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
                if (!me->isInFront(chaseTarget, M_PI / 2))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (circleCheckDelay < 0)
                {
                    me->GetMotionMaster()->MoveCircleTarget(chaseTarget);
                    circleCheckDelay = 20;
                }
                else
                {
                    circleCheckDelay -= pmDiff;
                }
                if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                {
                    ResetMovement();
                    break;
                }
                float chaseDistance = distanceMax;
                if (targetDistance < distanceMax)
                {
                    if (me->IsWithinLOSInMap(chaseTarget))
                    {
                        if (me->isMoving())
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                        }
                        if (!me->isInFront(chaseTarget, M_PI / 2))
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
                    if (ptDistance > distanceMax)
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
                //Position predict = sNierManager->PredictPosition(chaseTarget);                
                //chaseTarget->GetNearPoint(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, chaseDistance, chaseTarget->GetAngle(me), 0.0f, &predict);
                positionTarget = chaseTarget->GetPosition();
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

void NierMovement::Update_Chase(uint32 pmDiff)
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
    case NierMovementType::NierMovementType_None:
    {
        break;
    }
    case NierMovementType::NierMovementType_Point:
    {
        float distance = me->GetDistance(positionTarget);
        if (distance > VISIBILITY_DISTANCE_LARGE)
        {
            ResetMovement();
        }
        else
        {
            if (distance < CONTACT_DISTANCE)
            {
                activeMovementType = NierMovementType::NierMovementType_None;
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
    case NierMovementType::NierMovementType_Tank:
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
                if (!me->isInFront(chaseTarget, M_PI / 2))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float chaseDistance = distanceMax;
                if (chaseDistance > MELEE_RANGE)
                {
                    chaseDistance = chaseDistance - MELEE_RANGE;
                }
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (me->IsWithinLOSInMap(chaseTarget) && targetDistance < chaseTarget->GetCombatReach() + chaseDistance)
                {
                    if (!me->isInFront(chaseTarget, M_PI / 2))
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
    case NierMovementType::NierMovementType_Chase:
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
                if (!me->isInFront(chaseTarget, M_PI / 2))
                {
                    me->SetFacingToObject(chaseTarget);
                }
            }
            else
            {
                float chaseDistance = distanceMax;
                float targetDistance = me->GetExactDist(chaseTarget->GetPosition());
                if (me->IsWithinLOSInMap(chaseTarget) && targetDistance < chaseTarget->GetCombatReach() + me->GetCombatReach() + chaseDistance)
                {
                    if (!me->isInFront(chaseTarget, M_PI / 2))
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
                                return;
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
    case NierMovementType::NierMovementType_Follow:
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
                if (!me->isInFront(followTarget, M_PI / 2))
                {
                    me->SetFacingToObject(followTarget);
                }
            }
            else
            {
                float followDistance = distanceMax;
                float targetDistance = me->GetExactDist(followTarget->GetPosition());
                if (me->IsWithinLOSInMap(followTarget) && targetDistance < followTarget->GetCombatReach() + me->GetCombatReach() + followDistance)
                {
                    if (!me->isInFront(followTarget, M_PI / 2))
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
                                return;
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

bool NierMovement::Tank(Unit* pmTankTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    distanceMax = pmDistanceMax;
    distanceMin = pmDistanceMin;
    holding = pmHolding;
    if (me && pmTankTarget)
    {
        float distance = me->GetDistance(pmTankTarget);
        if (distance < VISIBILITY_DISTANCE_NORMAL)
        {
            activeMovementType = NierMovementType::NierMovementType_Tank;
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

bool NierMovement::Chase(Unit* pmChaseTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmForceBack)
{
    if (me && pmChaseTarget)
    {
        if (me->IsWithinDist(pmChaseTarget, VISIBILITY_DISTANCE_NORMAL))
        {
            distanceMax = pmDistanceMax;
            distanceMin = pmDistanceMin;
            holding = pmHolding;
            forceBack = pmForceBack;
            activeMovementType = NierMovementType::NierMovementType_Chase;
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

bool NierMovement::Follow(Unit* pmFollowTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    if (me && pmFollowTarget)
    {
        if (me->IsWithinDist(pmFollowTarget, VISIBILITY_DISTANCE_NORMAL))
        {
            distanceMax = pmDistanceMax;
            distanceMin = pmDistanceMin;
            holding = pmHolding;
            activeMovementType = NierMovementType::NierMovementType_Follow;
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

void NierMovement::Point(Position pmPosTarget, uint32 pmLimit)
{
    if (!me)
    {
        return;
    }
    float distance = me->GetDistance(pmPosTarget.m_positionX, pmPosTarget.m_positionY, pmPosTarget.m_positionZ);
    if (distance < VISIBILITY_DISTANCE_NORMAL)
    {
        activeMovementType = NierMovementType::NierMovementType_Point;
        if (me->IsNonMeleeSpellCast(false, false, true))
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

Position NierMovement::GetDestPosition(Unit* pmTarget, float pmMoveDistance, bool pmCloser)
{
    Position result;

    bool found = false;
    if (pmMoveDistance < VISIBILITY_DISTANCE_NORMAL)
    {
        float moveAngle = me->GetAbsoluteAngle(pmTarget);
        if (!pmCloser)
        {
            moveAngle += M_PI;
        }
        float checkDistance = pmMoveDistance;
        while (checkDistance < NIER_MAX_DISTANCE)
        {
            me->GetNearPoint(me, result.m_positionX, result.m_positionY, result.m_positionZ, 0.0f, checkDistance + CONTACT_DISTANCE, moveAngle);
            me->UpdateGroundPositionZ(result.m_positionX, result.m_positionY, result.m_positionZ);
            if (pmTarget->IsWithinLOS(result.m_positionX, result.m_positionY, result.m_positionZ))
            {
                found = true;
                break;
            }
            checkDistance += MELEE_RANGE;
        }
    }
    if (!found)
    {
        result = pmTarget->GetPosition();
    }

    return result;
}

bool NierMovement::Direction(Unit* pmCommander, uint32 pmDirection, uint32 pmLimit, float pmDistance)
{
    bool result = false;

    if (pmCommander)
    {
        float absAngle = pmCommander->GetOrientation();
        switch (pmDirection)
        {
        case NierMovementDirection_Forward:
        {
            break;
        }
        case NierMovementDirection_Back:
        {
            absAngle = absAngle + M_PI;
            break;
        }
        case NierMovementDirection_Left:
        {
            absAngle = absAngle + M_PI / 2;
            break;
        }
        case NierMovementDirection_Right:
        {
            absAngle = absAngle + M_PI * 3 / 2;
            break;
        }
        default:
        {
            break;
        }
        }
        me->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, pmDistance, absAngle);
        activeMovementType = NierMovementType::NierMovementType_Point;
        if (me->IsNonMeleeSpellCast(false, false, true))
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

bool NierMovement::Direction(float pmAngle, uint32 pmLimit, float pmDistance)
{
    bool result = false;

    me->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, 0.0f, pmDistance, pmAngle);
    activeMovementType = NierMovementType::NierMovementType_Point;
    Run();
    if (me->IsNonMeleeSpellCast(false, false, true))
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

NierAction_Base::NierAction_Base(Player* pmMe)
{
    me = pmMe;
    movement = new NierMovement(me);
    specialty = 0;
}

void NierAction_Base::Prepare()
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

void NierAction_Base::Reset()
{
    ClearTarget();
}

void NierAction_Base::Update(uint32 pmDiff)
{
    movement->Update_Direct(pmDiff);
    //movement->Update_Chase(pmDiff);
}

bool NierAction_Base::DPS(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack)
{
    return false;
}

bool NierAction_Base::AOE(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing)
{
    return false;
}

bool NierAction_Base::Tank(Unit* pmTarget, bool pmAOE, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
{
    return false;
}

bool NierAction_Base::Follow(Unit* pmFollowTarget, float pmDistanceMax, float pmDistanceMin, bool pmHolding)
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
    if (!movement->Follow(pmFollowTarget, pmDistanceMax, pmDistanceMin, pmHolding))
    {
        if (me->GetTarget() == pmFollowTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    ChooseTarget(pmFollowTarget);
    return true;
}

bool NierAction_Base::Heal(Unit* pmTarget, bool pmInstantOnly)
{
    return false;
}

bool NierAction_Base::ReadyTank(Unit* pmTarget)
{
    return false;
}

bool NierAction_Base::GroupHeal(Unit* pmTarget, bool pmInstantOnly)
{
    return false;
}

bool NierAction_Base::SimpleHeal(Unit* pmTarget, bool pmInstantOnly)
{
    return false;
}

bool NierAction_Base::Cure(Unit* pmTarget)
{
    return false;
}

bool NierAction_Base::Buff(Unit* pmTarget)
{
    return false;
}

uint32 NierAction_Base::Caution()
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
                me->GetNearPoint(me, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 0.0f, NIER_NORMAL_DISTANCE + MELEE_RANGE, startAngle + angleGap);
                if (me->GetDistance(pos.m_positionX, pos.m_positionY, pos.m_positionZ) > NIER_NORMAL_DISTANCE - CONTACT_DISTANCE)
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

bool NierAction_Base::Mark(Unit* pmTarget, int pmRTI)
{
    return false;
}

bool NierAction_Base::Assist(int pmRTI)
{
    return false;
}

bool NierAction_Base::Revive(Player* pmTarget)
{
    return false;
}

bool NierAction_Base::Petting(bool pmSummon, bool pmReset)
{
    return false;
}

void NierAction_Base::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
{

}

void NierAction_Base::ResetTalent()
{

}

bool NierAction_Base::InitializeEquipments(bool pmReset)
{
    return true;
}

void NierAction_Base::RemoveEquipments()
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

void NierAction_Base::LearnTalent(uint32 pmTalentId, uint32 pmMaxRank)
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

void NierAction_Base::TrainSpells(uint32 pmTrainerEntry)
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

void NierAction_Base::EquipRandomItem(uint32 pmEquipSlot, uint32 pmClass, uint32 pmSubclass, uint32 pmMinQuality, int pmModType)
{
    bool checkStat = true;
    if (pmModType < 0)
    {
        checkStat = false;
    }
    std::unordered_set<uint32> inventoryTypeSet;
    if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
    {
        inventoryTypeSet.insert(1);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
    {
        inventoryTypeSet.insert(3);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS)
    {
        inventoryTypeSet.insert(9);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST)
    {
        inventoryTypeSet.insert(6);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET)
    {
        inventoryTypeSet.insert(8);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
    {
        inventoryTypeSet.insert(10);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST)
    {
        inventoryTypeSet.insert(5);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS)
    {
        inventoryTypeSet.insert(7);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_BACK)
    {
        inventoryTypeSet.insert(16);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
    {
        inventoryTypeSet.insert(2);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1)
    {
        inventoryTypeSet.insert(11);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
    {
        inventoryTypeSet.insert(11);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_MAINHAND)
    {
        inventoryTypeSet.insert(InventoryType::INVTYPE_WEAPON);
        inventoryTypeSet.insert(InventoryType::INVTYPE_2HWEAPON);
        inventoryTypeSet.insert(InventoryType::INVTYPE_WEAPONMAINHAND);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_OFFHAND)
    {
        inventoryTypeSet.insert(InventoryType::INVTYPE_WEAPON);
        inventoryTypeSet.insert(InventoryType::INVTYPE_WEAPONOFFHAND);
        inventoryTypeSet.insert(InventoryType::INVTYPE_HOLDABLE);
        inventoryTypeSet.insert(InventoryType::INVTYPE_SHIELD);
    }
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
    {
        inventoryTypeSet.insert(15);
        inventoryTypeSet.insert(InventoryType::INVTYPE_RANGEDRIGHT);
        inventoryTypeSet.insert(InventoryType::INVTYPE_THROWN);
    }
    else
    {
        return;
    }

    std::ostringstream inventoryTypeQueryStream;
    inventoryTypeQueryStream << "0";
    for (std::unordered_set<uint32>::iterator itIT = inventoryTypeSet.begin(); itIT != inventoryTypeSet.end(); itIT++)
    {
        inventoryTypeQueryStream << "," << (*itIT);
    }
    std::string inventoryTypeQueryStr = inventoryTypeQueryStream.str();

    int maxReqLevel = me->getLevel();
    int minReqLevel = maxReqLevel - 5;
    while (minReqLevel > 0 && maxReqLevel > 1)
    {
        std::ostringstream itemQueryStream;
        itemQueryStream << "SELECT entry FROM item_template where class = " << pmClass << " and subclass = " << pmSubclass << " and InventoryType in (" << inventoryTypeQueryStr << ") and RequiredSkill = 0 and RequiredReputationFaction = 0 and RequiredLevel >= " << minReqLevel << " and RequiredLevel <= " << maxReqLevel << " order by rand() ";
        auto itemQR = WorldDatabase.Query(itemQueryStream.str().c_str());
        if (itemQR)
        {
            do
            {
                Field* fields = itemQR->Fetch();
                uint32 itemEntry = fields[0].Get<uint32>();

                if (Item* pItem = Item::CreateItem(itemEntry, 1))
                {
                    uint16 dest = 0;
                    me->CombatStopWithPets(true);
                    if (me->CanEquipItem(pmEquipSlot, dest, pItem, false) == InventoryResult::EQUIP_ERR_OK)
                    {
                        me->EquipItem(dest, pItem, true);
                        std::ostringstream msgStream;
                        msgStream << me->GetName() << " Equiped " << pItem->GetTemplate()->Name1;
                        //sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
                        sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, msgStream.str().c_str());
                        return;
                    }
                }
            } while (itemQR->NextRow());
        }
        maxReqLevel = maxReqLevel - 1;
        minReqLevel = maxReqLevel - 5;
    }
}

void NierAction_Base::PetAttack(Unit* pmTarget)
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

void NierAction_Base::PetStop()
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

bool NierAction_Base::UseItem(Item* pmItem, Unit* pmTarget)
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
        //ChooseTarget(pmTarget);
        SpellCastTargets targets;
        targets.Update(pmTarget);
        me->CastItemUseSpell(pmItem, targets, 1, 0);
        return true;
    }

    return false;
}

bool NierAction_Base::UseItem(Item* pmItem, Item* pmTarget)
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

bool NierAction_Base::CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura, bool pmOnlyMyAura, bool pmClearShapeShift, uint32 pmMaxAuraStack)
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
            if (!me->isInFront(pmTarget, M_PI / 2))
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

        SpellCastResult scr = me->CastSpell(pmTarget, pS->Id, TriggerCastFlags::TRIGGERED_NONE);
        if (scr == SpellCastResult::SPELL_CAST_OK)
        {
            return true;
        }
    }

    return false;
}

void NierAction_Base::CancelAura(uint32 pmSpellID)
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

bool NierAction_Base::Eat()
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
            movement->ResetMovement();
            return true;
        }
    }
    return false;
}

bool NierAction_Base::Drink()
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
            movement->ResetMovement();
            return true;
        }
    }

    return false;
}

bool NierAction_Base::HealthPotion()
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

bool NierAction_Base::ManaPotion()
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

void NierAction_Base::ChooseTarget(Unit* pmTarget)
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

void NierAction_Base::ClearTarget()
{
    if (me)
    {
        me->SetSelection(ObjectGuid::Empty);
        me->SetTarget(ObjectGuid::Empty);
        me->AttackStop();
        me->InterruptNonMeleeSpells(true);
    }
}

bool NierAction_Base::SpellValid(uint32 pmSpellID)
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

Item* NierAction_Base::GetItemInInventory(uint32 pmEntry)
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

Player* NierAction_Base::GetNearbyHostilePlayer(float pmRange)
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

Unit* NierAction_Base::GetNearbyHostileUnit(float pmRange)
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
