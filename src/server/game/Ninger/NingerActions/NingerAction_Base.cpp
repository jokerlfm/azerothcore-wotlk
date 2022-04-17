#include "NingerAction_Base.h"
#include "Bag.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "CreatureAI.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "MoveSplineInit.h"
#include "SpellMgr.h"
#include "Pet.h"
#include "GridNotifiers.h"

NingerMovement::NingerMovement(Player* pmMe)
{
    me = pmMe;
    ogTankTarget = ObjectGuid::Empty;
    ogChaseTarget = ObjectGuid::Empty;
    ogFollowTarget = ObjectGuid::Empty;
    positionTarget = me->GetPosition();
    forceBack = false;
    positionOK = false;
    movingDelay = 0;
    activeMovementType = NingerMovementType::NingerMovementType_None;
}

void NingerMovement::ResetMovement()
{
    me->StopMoving();
    me->GetMotionMaster()->Clear();
    ogTankTarget = ObjectGuid::Empty;
    ogChaseTarget = ObjectGuid::Empty;
    ogFollowTarget = ObjectGuid::Empty;
    positionTarget = me->GetPosition();
    forceBack = false;
    positionOK = false;
    movingDelay = 0;
    activeMovementType = NingerMovementType::NingerMovementType_None;
}

void NingerMovement::Update(uint32 pmDiff)
{
    if (movingDelay > 0)
    {
        movingDelay -= pmDiff;
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
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return;
    }
    if (me->IsBeingTeleported())
    {
        return;
    }
    if (me->isMoving())
    {
        if (movingDelay > 0)
        {
            return;
        }
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
                if (me->isMoving())
                {
                    me->StopMoving();
                }
            }
            else
            {
                me->GetMap()->CanReachPositionAndGetValidCoords(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, false, false);
                me->GetMotionMaster()->MovePoint(0, positionTarget);
                movingDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
            }
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Tank:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogTankTarget))
        {
            if (NingerStrategy_Base* ns = me->strategyMap[me->activeStrategyIndex])
            {
                if (chaseTarget->isMoving())
                {
                    positionOK = false;
                }
                if (!positionOK)
                {
                    float targetDistance = me->GetDistance2d(chaseTarget);
                    if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                    {
                        ResetMovement();
                        break;
                    }
                    if (me->GetTarget() != ogTankTarget)
                    {
                        me->SetSelection(ogTankTarget);
                        me->SetTarget(ogTankTarget);
                    }
                    if (targetDistance < ns->dpsDistance + MIN_DISTANCE_GAP)
                    {
                        if (me->IsWithinLOSInMap(chaseTarget))
                        {
                            if (ns->combatAngle != 0.0f)
                            {
                                float targetAngle = me->GetAngle(chaseTarget);
                                if (targetAngle < 0.0f)
                                {
                                    targetAngle = targetAngle + M_PI * 2;
                                }
                                targetAngle = targetAngle + M_PI * 2;
                                if (targetAngle > ns->combatAngle + M_PI * 2 + M_PI - MIN_ANGLE_GAP && targetAngle < ns->combatAngle + M_PI * 2 + M_PI + MIN_ANGLE_GAP)
                                {
                                    positionOK = true;
                                }
                            }
                            else
                            {
                                positionOK = true;
                            }
                        }
                    }
                }
                if (positionOK)
                {
                    if (me->isMoving())
                    {
                        me->StopMoving();
                    }
                    if (!me->isInFront(chaseTarget, M_PI / 4))
                    {
                        me->SetFacingToObject(chaseTarget);
                    }
                }
                else
                {
                    float chaseAngle = ns->combatAngle;
                    if (chaseAngle == 0.0f)
                    {
                        chaseAngle = chaseTarget->GetAngle(me);
                        if (Group* myGroup = me->GetGroup())
                        {
                            if (myGroup->GetLeaderGUID() != me->GetGUID())
                            {
                                if (Player* leader = myGroup->GetLeader())
                                {
                                    chaseAngle = leader->GetOrientation();
                                }
                            }
                        }
                    }
                    chaseTarget->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, me->GetObjectSize(), ns->dpsDistance, chaseAngle);
                    me->GetMap()->CanReachPositionAndGetValidCoords(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, false, false);
                    if (me->GetDistance(positionTarget) > MIN_DISTANCE_GAP)
                    {
                        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
                        {
                            me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
                        }
                        if (me->IsWalking())
                        {
                            me->SetWalk(false);
                        }
                        me->GetMotionMaster()->MovePoint(0, positionTarget);
                        movingDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
                    }
                }
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
            if (NingerStrategy_Base* ns = me->strategyMap[me->activeStrategyIndex])
            {
                Position targetPredict = chaseTarget->GetPosition();
                if (chaseTarget->isMoving())
                {
                    positionOK = false;
                    targetPredict = PredictPosition(chaseTarget);
                }
                if (!positionOK)
                {
                    float targetDistance = me->GetDistance2d(chaseTarget);
                    if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                    {
                        ResetMovement();
                        break;
                    }
                    if (me->GetTarget() != ogChaseTarget)
                    {
                        me->SetSelection(ogChaseTarget);
                        me->SetTarget(ogChaseTarget);
                    }
                    if (targetDistance < ns->dpsDistance + MIN_DISTANCE_GAP)
                    {
                        if (me->IsWithinLOSInMap(chaseTarget))
                        {
                            positionOK = true;
                            if (chaseTarget->GetTarget() != me->GetGUID())
                            {
                                if (forceBack)
                                {
                                    if (!chaseTarget->isInBack(me, M_PI / 4))
                                    {
                                        positionOK = false;
                                    }
                                }
                                else if (ns->combatAngle != 0.0f)
                                {
                                    float targetAngle = me->GetAngle(chaseTarget);
                                    if (targetAngle < 0.0f)
                                    {
                                        targetAngle = targetAngle + M_PI * 2;
                                    }
                                    targetAngle = targetAngle + M_PI * 2;
                                    if (targetAngle > ns->combatAngle + M_PI * 2 - MIN_ANGLE_GAP && targetAngle < ns->combatAngle + M_PI * 2 + MIN_ANGLE_GAP)
                                    {
                                        positionOK = true;
                                    }
                                }
                            }
                        }
                    }
                }
                if (positionOK)
                {
                    if (me->isMoving())
                    {
                        me->StopMoving();
                    }
                    if (!me->isInFront(chaseTarget, M_PI / 4))
                    {
                        me->SetFacingToObject(chaseTarget);
                    }
                }
                else
                {
                    float chaseAngle = chaseTarget->GetAngle(me) + M_PI * 2;
                    if (forceBack)
                    {
                        chaseAngle = chaseTarget->GetOrientation() + M_PI * 2 + M_PI;
                    }
                    else if (ns->combatAngle != 0.0f)
                    {
                        chaseAngle = ns->combatAngle;
                        chaseAngle = chaseAngle + M_PI;
                    }
                    chaseAngle = frand(chaseAngle - M_PI / 8, chaseAngle + M_PI / 8);
                    chaseTarget->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, me->GetObjectSize(), ns->dpsDistance, chaseAngle, 0.0f, &targetPredict);
                    me->GetMap()->CanReachPositionAndGetValidCoords(chaseTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, false, false);
                    if (me->GetDistance(positionTarget) > MIN_DISTANCE_GAP)
                    {
                        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
                        {
                            me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
                        }
                        if (me->IsWalking())
                        {
                            me->SetWalk(false);
                        }
                        me->GetMotionMaster()->MovePoint(0, positionTarget);
                        movingDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
                    }
                }
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
        if (Unit* followTarget = ObjectAccessor::GetUnit(*me, ogFollowTarget))
        {
            if (NingerStrategy_Base* ns = me->strategyMap[me->activeStrategyIndex])
            {
                Position targetPredict = followTarget->GetPosition();
                if (followTarget->isMoving())
                {
                    positionOK = false;
                    targetPredict = PredictPosition(followTarget);
                }
                if (!positionOK)
                {
                    float targetDistance = me->GetDistance2d(followTarget);
                    if (targetDistance > VISIBILITY_DISTANCE_LARGE)
                    {
                        ResetMovement();
                        break;
                    }
                    if (me->GetTarget() != ogFollowTarget)
                    {
                        me->SetSelection(ogFollowTarget);
                        me->SetTarget(ogFollowTarget);
                    }
                    if (targetDistance < ns->followDistance + MIN_DISTANCE_GAP)
                    {
                        if (me->IsWithinLOSInMap(followTarget))
                        {
                            if (ns->combatAngle != 0.0f)
                            {
                                float targetAngle = me->GetAngle(followTarget);
                                if (targetAngle < 0.0f)
                                {
                                    targetAngle = targetAngle + M_PI * 2;
                                }
                                targetAngle = targetAngle + M_PI * 2;
                                if (targetAngle > ns->combatAngle + M_PI * 2 - MIN_ANGLE_GAP && targetAngle < ns->combatAngle + M_PI * 2 + MIN_ANGLE_GAP)
                                {
                                    positionOK = true;
                                }
                            }
                            positionOK = true;
                        }
                    }
                }
                if (positionOK)
                {
                    if (me->isMoving())
                    {
                        me->StopMoving();
                    }
                }
                else
                {
                    float followAngle = followTarget->GetOrientation() + M_PI + M_PI * 2;
                    if (ns->combatAngle != 0.0f)
                    {
                        followAngle = ns->combatAngle + M_PI + M_PI * 2;
                    }
                    followAngle = frand(followAngle - MIN_ANGLE_GAP, followAngle + MIN_ANGLE_GAP);
                    followTarget->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, me->GetObjectSize(), ns->followDistance, followAngle, 0.0f, &targetPredict);
                    me->GetMap()->CanReachPositionAndGetValidCoords(followTarget, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, false, false);
                    if (me->GetDistance(positionTarget) > MIN_DISTANCE_GAP)
                    {
                        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
                        {
                            me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
                        }
                        if (me->IsWalking())
                        {
                            me->SetWalk(false);
                        }
                        me->GetMotionMaster()->MovePoint(0, positionTarget);
                        movingDelay = DEFAULT_MOVEMENT_UPDATE_DELAY;
                    }
                }
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

Position NingerMovement::PredictPosition(Unit* target)
{
    Position pos = target->GetPosition();

    // 0.5 - it's time (0.5 sec) between starting movement opcode (e.g. MSG_MOVE_START_FORWARD) and MSG_MOVE_HEARTBEAT sent by client
    float speed = target->GetSpeed(Movement::SelectSpeedType(target->GetUnitMovementFlags())) * 0.5f;
    float orientation = target->GetOrientation();

    if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FORWARD))
    {
        pos.m_positionX += cos(orientation) * speed;
        pos.m_positionY += std::sin(orientation) * speed;
    }
    else if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_BACKWARD))
    {
        pos.m_positionX -= cos(orientation) * speed;
        pos.m_positionY -= std::sin(orientation) * speed;
    }

    if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_STRAFE_LEFT))
    {
        pos.m_positionX += cos(orientation + M_PI / 2.f) * speed;
        pos.m_positionY += std::sin(orientation + M_PI / 2.f) * speed;
    }
    else if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT))
    {
        pos.m_positionX += cos(orientation - M_PI / 2.f) * speed;
        pos.m_positionY += std::sin(orientation - M_PI / 2.f) * speed;
    }

    return pos;
}

bool NingerMovement::Tank(Unit* pmTankTarget)
{
    if (me && pmTankTarget)
    {
        float distance = me->GetDistance(pmTankTarget);
        if (distance < VISIBILITY_DISTANCE_NORMAL)
        {
            activeMovementType = NingerMovementType::NingerMovementType_Tank;
            if (ogTankTarget != pmTankTarget->GetGUID())
            {
                ogTankTarget = pmTankTarget->GetGUID();
                positionOK = false;
            }
            return true;
        }
    }

    return false;
}

bool NingerMovement::Chase(Unit* pmChaseTarget, bool pmForceBack)
{
    if (me && pmChaseTarget)
    {
        float distance = me->GetDistance(pmChaseTarget);
        if (distance < VISIBILITY_DISTANCE_NORMAL)
        {
            forceBack = pmForceBack;
            activeMovementType = NingerMovementType::NingerMovementType_Chase;
            if (ogChaseTarget != pmChaseTarget->GetGUID())
            {
                ogChaseTarget = pmChaseTarget->GetGUID();
                positionOK = false;
            }
            return true;
        }
    }

    return false;
}

bool NingerMovement::Follow(Unit* pmFollowTarget)
{
    if (me && pmFollowTarget)
    {
        float distance = me->GetDistance(pmFollowTarget);
        if (distance < VISIBILITY_DISTANCE_NORMAL)
        {
            activeMovementType = NingerMovementType::NingerMovementType_Follow;
            if (ogFollowTarget != pmFollowTarget->GetGUID())
            {
                ogFollowTarget = pmFollowTarget->GetGUID();
                positionOK = false;
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
    float distance = me->GetDistance(pmPosTarget);
    if (distance < VISIBILITY_DISTANCE_NORMAL)
    {
        activeMovementType = NingerMovementType::NingerMovementType_Point;
        positionTarget = pmPosTarget;
        positionOK = false;
        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
        {
            me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
        }
        if (me->IsWalking())
        {
            me->SetWalk(false);
        }
        if (me->IsNonMeleeSpellCast(false))
        {
            me->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
            me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
        }
        me->StopMoving();
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MovePoint(0, positionTarget);
    }
}

bool NingerMovement::Direction(Player* pmCommander, uint32 pmDirection, uint32 pmLimit)
{
    bool result = false;

    if (pmCommander)
    {
        if (pmCommander->GetDistance(me) < FOLLOW_MAX_DISTANCE)
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
                absAngle = absAngle + M_PI_2;
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
            me->GetNearPoint(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, me->GetObjectSize(), FOLLOW_NEAR_DISTANCE, absAngle);
            //me->GetMap()->CanReachPositionAndGetValidCoords(me, positionTarget.m_positionX, positionTarget.m_positionY, positionTarget.m_positionZ, false, false);
            activeMovementType = NingerMovementType::NingerMovementType_Point;
            positionOK = false;
            if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
            {
                me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
            }
            if (me->IsWalking())
            {
                me->SetWalk(false);
            }
            if (me->IsNonMeleeSpellCast(false))
            {
                me->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
            }
            me->StopMoving();
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(0, positionTarget);
            result = true;
        }
    }

    return result;
}

NingerAction_Base::NingerAction_Base()
{
    me = nullptr;
    specialty = 0;
    rti = -1;
}

void NingerAction_Base::Prepare()
{
    if (me)
    {
        me->SetPvP(true);
        me->DurabilityRepairAll(false, 0, false);
    }
}

void NingerAction_Base::Reset()
{
    rti = -1;
    ClearTarget();
}

void NingerAction_Base::Update(uint32 pmDiff)
{

}

bool NingerAction_Base::DPS(Unit* pmTarget, bool pmAOE)
{
    return false;
}

bool NingerAction_Base::Tank(Unit* pmTarget, bool pmAOE)
{
    return false;
}

bool NingerAction_Base::Heal(Unit* pmTarget)
{
    return false;
}

bool NingerAction_Base::SimpleHeal(Unit* pmTarget)
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

bool NingerAction_Base::Assist(Unit* pmTarget)
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

void NingerAction_Base::InitializeEquipments(bool pmReset)
{

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

void NingerAction_Base::EquipRandomItem(uint32 pmEquipSlot, uint32 pmClass, uint32 pmSubclass, uint32 pmMinQuality, uint32 pmMaxRequiredLevel, int pmModType, std::unordered_set<uint32> pmInventoryTypeSet)
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

    uint32 minRequiredLevel = pmMaxRequiredLevel;
    if (minRequiredLevel > 20)
    {
        minRequiredLevel = minRequiredLevel - 10;
    }
    else if (minRequiredLevel > 10)
    {
        minRequiredLevel = 10;
    }
    else
    {
        minRequiredLevel = 0;
    }
    std::unordered_set<uint32> availableItemEntrySet;
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
    int activeQuality = pmMinQuality;
    while (activeQuality >= 0)
    {
        QueryResult itemQR = WorldDatabase.Query("SELECT entry FROM item_template where {} and class = {} and subclass = {} and Quality >= {} and RequiredLevel <= {} and RequiredLevel >= {} and AllowableClass = -1 and AllowableRace = -1 and RequiredSkill = 0 and requiredspell = 0 and requiredhonorrank = 0 and RequiredCityRank = 0 and RequiredReputationFaction = 0 order by rand()", inventoryTypeQuery, pmClass, pmSubclass, activeQuality, pmMaxRequiredLevel, minRequiredLevel);
        if (itemQR)
        {
            do
            {
                Field* fields = itemQR->Fetch();
                availableItemEntrySet.insert(fields[0].Get<uint32>());
            } while (itemQR->NextRow());
        }
        activeQuality--;
    }

    for (std::unordered_set<uint32>::iterator entryIt = availableItemEntrySet.begin(); entryIt != availableItemEntrySet.end(); entryIt++)
    {
        if (const ItemTemplate* pProto = sObjectMgr->GetItemTemplate(*entryIt))
        {
            bool hasStat = false;
            if (checkStat)
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
                        break;
                    }
                }
            }
        }
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
        me->SetSelection(pmTarget->GetGUID());
        me->SetTarget(pmTarget->GetGUID());
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
        ClearShapeshift();
    }
    const SpellInfo* pS = sSpellMgr->GetSpellInfo(pmSpellId);
    if (!pS)
    {
        return false;
    }
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
            if (pmOnlyMyAura)
            {
                if (pmTarget->HasAura(pmSpellId, me->GetGUID()))
                {
                    if (pmMaxAuraStack > 1)
                    {
                        if (Aura* activeAura = pmTarget->GetAura(pmSpellId, me->GetGUID()))
                        {
                            if (activeAura->GetStackAmount() >= pmMaxAuraStack)
                            {
                                return false;
                            }
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (pmTarget->HasAura(pmSpellId))
                {
                    if (pmMaxAuraStack > 1)
                    {
                        if (Aura* activeAura = pmTarget->GetAura(pmSpellId))
                        {
                            if (activeAura->GetStackAmount() >= pmMaxAuraStack)
                            {
                                return false;
                            }
                        }
                    }
                    else
                    {
                        return false;
                    }
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
    me->CastSpell(pmTarget, pS);
    return true;

    //SpellCastResult scr = me->CastSpell(pmTarget, pS);
    //if (scr == SpellCastResult::SPELL_CAST_OK)
    //{
    //    return true;
    //}
    //return false;
}

void NingerAction_Base::ClearShapeshift()
{
    if (!me)
    {
        return;
    }
    me->RemoveAurasByShapeShift();
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
    if (const SpellInfo* pS = sSpellMgr->GetSpellInfo(pmSpellID))
    {
        if (const SpellInfo* pSFirst = pS->GetFirstRankSpell())
        {
            me->RemoveAurasDueToSpell(pSFirst->Id);
            while (true)
            {
                if (const SpellInfo* pSNext = pSFirst->GetNextRankSpell())
                {
                    me->RemoveAurasDueToSpell(pSNext->Id);
                }
                else
                {
                    break;
                }
            }
        }
    }
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
    me->GetMotionMaster()->Initialize();
    ClearTarget();

    Item* pFood = GetItemInInventory(foodEntry);
    if (pFood && !pFood->IsInTrade())
    {
        if (UseItem(pFood, me))
        {
            me->ningerMovement->ResetMovement();
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
    me->GetMotionMaster()->Initialize();
    me->StopMoving();
    ClearTarget();
    Item* pDrink = GetItemInInventory(drinkEntry);
    if (pDrink && !pDrink->IsInTrade())
    {
        if (UseItem(pDrink, me))
        {
            me->ningerMovement->ResetMovement();
            return true;
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
    if (Player* targetPlayer = ObjectAccessor::FindConnectedPlayer(samePlayersMap[targetPlayerIndex]))
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
                    return eachPlayer;
                }
            }
        }
    }

    return nullptr;
}

Unit* NingerAction_Base::GetNearbyHostileUnit(float pmRange)
{
    std::list<Creature*> creatureList;
    Acore::AnyUnitInObjectRangeCheck go_check(me, pmRange);
    Acore::CreatureListSearcher<Acore::AnyUnitInObjectRangeCheck> go_search(me, creatureList, go_check);
    Cell::VisitGridObjects(me, go_search, pmRange);
    if (!creatureList.empty())
    {
        for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
        {
            if (Unit* eachUnit = *itr)
            {
                if (eachUnit->IsAlive())
                {
                    if (Creature* hostileCreature = eachUnit->ToCreature())
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
    }

    return nullptr;
}

Unit* NingerAction_Base::GetAnyUnitInRange(float pmMinRange, float pmMaxRange)
{
    std::unordered_map<uint32, Unit*> unitsMap;

    std::list<Creature*> creatureList;
    Acore::AnyUnitInObjectRangeCheck go_check(me, pmMaxRange);
    Acore::CreatureListSearcher<Acore::AnyUnitInObjectRangeCheck> go_search(me, creatureList, go_check);
    Cell::VisitGridObjects(me, go_search, pmMaxRange);
    if (!creatureList.empty())
    {
        for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
        {
            if (Unit* eachUnit = *itr)
            {
                float eachDistance = me->GetDistance(eachUnit);
                if (eachDistance > pmMinRange)
                {
                    unitsMap[unitsMap.size()] = eachUnit;
                }
            }
        }
    }

    if (unitsMap.size() > 0)
    {
        uint32 targetIndex = urand(0, unitsMap.size() - 1);
        return unitsMap[targetIndex];
    }

    return nullptr;
}
