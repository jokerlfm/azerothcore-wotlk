#include "NingerAction_Base.h"
#include "Bag.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "CreatureAI.h"
#include "MotionMaster.h"
#include "SpellMgr.h"
#include "Pet.h"
#include "GridNotifiers.h"

NingerMovement::NingerMovement(Player* pmMe)
{
    me = pmMe;
    ogChaseTarget = ObjectGuid::Empty;
    positionTarget = me->GetPosition();
    activeMovementType = NingerMovementType::NingerMovementType_None;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
}

void NingerMovement::ResetMovement()
{
    me->StopMoving();
    me->GetMotionMaster()->Clear();
    ogChaseTarget = ObjectGuid::Empty;
    positionTarget = me->GetPosition();
    activeMovementType = NingerMovementType::NingerMovementType_None;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
}

void NingerMovement::Update(uint32 pmDiff)
{
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
    if (me->IsNonMeleeSpellCast(false))
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
        if (distance > VISIBILITY_DISTANCE_LARGE || distance < CONTACT_DISTANCE)
        {
            ResetMovement();
        }
        else
        {
            if (!me->isMoving())
            {
                MoveTargetPosition();
            }
        }
        break;
    }
    case NingerMovementType::NingerMovementType_Chase:
    {
        if (Unit* chaseTarget = ObjectAccessor::GetUnit(*me, ogChaseTarget))
        {
            if (me->GetMapId() != chaseTarget->GetMapId())
            {
                ResetMovement();
                break;
            }
            if (chaseTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
            {
                if (Player* targetPlayer = chaseTarget->ToPlayer())
                {
                    if (!targetPlayer->IsInWorld())
                    {
                        ResetMovement();
                        break;
                    }
                    else if (targetPlayer->IsBeingTeleported())
                    {
                        ResetMovement();
                        break;
                    }
                }
            }
            float unitTargetDistance = me->GetDistance(chaseTarget);
            if (unitTargetDistance > VISIBILITY_DISTANCE_LARGE)
            {
                ResetMovement();
                break;
            }
            if (me->GetTarget() != ogChaseTarget)
            {
                me->SetSelection(ogChaseTarget);
                me->SetTarget(ogChaseTarget);
            }
            bool ok = false;
            if (unitTargetDistance >= chaseDistanceMin && unitTargetDistance <= chaseDistanceMax + MIN_DISTANCE_GAP)
            {
                if (me->IsWithinLOSInMap(chaseTarget))
                {
                    if (me->isMoving())
                    {
                        me->StopMoving();
                    }
                    if (!me->isInFront(chaseTarget, M_PI / 4))
                    {
                        me->SetFacingToObject(chaseTarget);
                    }
                    ok = true;
                }
            }
            if (!ok)
            {
                if (me->isMoving())
                {
                    ok = true;
                }
            }
            if (!ok)
            {
                if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
                {
                    me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
                }
                float distanceInRange = frand(chaseDistanceMin, chaseDistanceMax);
                float nearX = 0, nearY = 0, nearZ = 0;
                float dynamicAngle = M_PI / 16;
                if (distanceInRange < INTERACTION_DISTANCE)
                {
                    dynamicAngle = M_PI / 8;
                }
                float chaseAngle = chaseTarget->GetAngle(me);
                chaseAngle = frand(chaseAngle - dynamicAngle, chaseAngle + dynamicAngle);
                chaseTarget->GetNearPoint(me, nearX, nearY, nearZ, me->GetCombatReach(), distanceInRange, chaseAngle);
                MoveTargetPosition(nearX, nearY, nearZ);
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

bool NingerMovement::Chase(Unit* pmChaseTarget, float pmChaseDistanceMin, float pmChaseDistanceMax, uint32 pmLimitDelay)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
    {
        return false;
    }
    if (me->HasUnitState(UnitState::UNIT_STATE_NOT_MOVE))
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false))
    {
        return true;
    }
    if (!pmChaseTarget)
    {
        return false;
    }
    if (me->GetMapId() != pmChaseTarget->GetMapId())
    {
        return false;
    }
    float unitTargetDistance = me->GetDistance(pmChaseTarget);
    if (unitTargetDistance > VISIBILITY_DISTANCE_LARGE)
    {
        return false;
    }
    if (pmChaseTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
    {
        if (Player* targetPlayer = pmChaseTarget->ToPlayer())
        {
            if (targetPlayer->IsBeingTeleported())
            {
                return false;
            }
        }
    }
    chaseDistanceMin = pmChaseDistanceMin;
    chaseDistanceMax = pmChaseDistanceMax;
    if (activeMovementType == NingerMovementType::NingerMovementType_Chase)
    {
        if (ogChaseTarget == pmChaseTarget->GetGUID())
        {
            return true;
        }
    }
    ResetMovement();
    ogChaseTarget = pmChaseTarget->GetGUID();
    activeMovementType = NingerMovementType::NingerMovementType_Chase;
    if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
    {
        me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
    }
    bool distanceValid = false;
    if (unitTargetDistance < chaseDistanceMax + MIN_DISTANCE_GAP)
    {
        if (chaseDistanceMin > MELEE_MIN_DISTANCE)
        {
            if (unitTargetDistance > chaseDistanceMin)
            {
                distanceValid = true;
            }
            else
            {
                distanceValid = false;
            }
        }
        else
        {
            distanceValid = true;
        }
    }
    if (distanceValid)
    {
        if (me->IsWithinLOSInMap(pmChaseTarget))
        {
            if (!me->isInFront(pmChaseTarget, M_PI / 4))
            {
                me->SetFacingToObject(pmChaseTarget);
            }
        }
    }
    else
    {
        float distanceInRange = frand(chaseDistanceMin, chaseDistanceMax);
        float nearX = 0, nearY = 0, nearZ = 0;
        float dynamicAngle = M_PI / 16;
        if (distanceInRange < INTERACTION_DISTANCE)
        {
            dynamicAngle = M_PI / 8;
        }
        float chaseAngle = pmChaseTarget->GetAngle(me);
        chaseAngle = frand(chaseAngle - dynamicAngle, chaseAngle + dynamicAngle);
        pmChaseTarget->GetNearPoint(me, nearX, nearY, nearZ, me->GetCombatReach(), distanceInRange, chaseAngle);
        MoveTargetPosition(nearX, nearY, nearZ);
    }
    return true;
}

void NingerMovement::MovePoint(Position pmTargetPosition, uint32 pmLimitDelay)
{
    MovePoint(pmTargetPosition.m_positionX, pmTargetPosition.m_positionY, pmTargetPosition.m_positionZ, pmLimitDelay);
}

void NingerMovement::MovePoint(float pmX, float pmY, float pmZ, uint32 pmLimitDelay)
{
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
    if (me->IsNonMeleeSpellCast(false))
    {
        return;
    }
    if (me->IsBeingTeleported())
    {
        ResetMovement();
        return;
    }
    activeMovementType = NingerMovementType::NingerMovementType_Point;
    MoveTargetPosition(pmX, pmY, pmZ);
}

void NingerMovement::MoveTargetPosition()
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
        me->GetMotionMaster()->MovePoint(1, positionTarget);
    }
}

void NingerMovement::MoveTargetPosition(float pmX, float pmY, float pmZ)
{
    positionTarget.m_positionX = pmX;
    positionTarget.m_positionY = pmY;
    positionTarget.m_positionZ = pmZ;
    MoveTargetPosition();
}


NingerAction_Base::NingerAction_Base()
{
    me = nullptr;
    rm = nullptr;
    maxTalentTab = 0;
    if (me)
    {
        maxTalentTab = me->GetMostPointsTalentTree();
    }
    if (rm)
    {
        rm->ResetMovement();
    }
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
    rti = -1;
}

void NingerAction_Base::Prepare()
{

}

void NingerAction_Base::Reset()
{
    rti = -1;
    if (me)
    {
        maxTalentTab = me->GetMostPointsTalentTree();
    }
    if (rm)
    {
        rm->ResetMovement();
    }
    maxTalentTab = 0;
    chaseDistanceMin = 1.0f;
    chaseDistanceMax = 3.0f;
    rti = -1;
    ClearTarget();
}

void NingerAction_Base::Update(uint32 pmDiff)
{

}

bool NingerAction_Base::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    return false;
}

bool NingerAction_Base::Tank(Unit* pmTarget, bool pmChase, bool pmAOE)
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

bool NingerAction_Base::Petting(bool pmSummon)
{
    return false;
}

void NingerAction_Base::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
{

}

void NingerAction_Base::InitializeEquipments(bool pmReset)
{

}

void NingerAction_Base::HandleChatCommand(Player* pmSender, std::string pmCMD)
{
    std::vector<std::string> commandVector = sNingerManager->SplitString(pmCMD, " ", true);
    std::string commandName = commandVector.at(0);
    if (commandName == "role")
    {
        std::ostringstream replyStream;
        if (commandVector.size() > 1)
        {
            std::string newRole = commandVector.at(1);
            if (newRole == "tank")
            {
                me->groupRole = GroupRole::GroupRole_Tank;
            }
            else if (newRole == "healer")
            {
                me->groupRole = GroupRole::GroupRole_Healer;
            }
            else if (newRole == "dps")
            {
                me->groupRole = GroupRole::GroupRole_DPS;
            }
        }
        if (me->groupRole == GroupRole::GroupRole_Tank)
        {
            replyStream << "My group role is tank";
        }
        else if (me->groupRole == GroupRole::GroupRole_Healer)
        {
            replyStream << "My group role is healer";
        }
        else if (me->groupRole == GroupRole::GroupRole_DPS)
        {
            replyStream << "My group role is dps";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "engage")
    {
        if (NingerStrategy_Base* ns = me->strategyMap[me->activeStrategyIndex])
        {
            ns->chase = true;
            ns->follow = true;
            if (Unit* target = pmSender->GetSelectedUnit())
            {
                if (ns->Engage(target))
                {
                    int engageLimit = DEFAULT_ENGAGE_LIMIT_DELAY;
                    if (commandVector.size() > 1)
                    {
                        std::string checkStr = commandVector.at(1);
                        engageLimit = atoi(checkStr.c_str());
                    }
                    ns->engageLimit = engageLimit;
                    std::ostringstream replyStream;
                    replyStream << "Try to engage " << target->GetName();                    
                    me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
                }
            }
        }
    }
    else if (commandName == "tank")
    {
        if (NingerStrategy_Base* ns = me->strategyMap[me->activeStrategyIndex])
        {
            ns->chase = true;
            ns->follow = true;
            if (Unit* target = pmSender->GetSelectedUnit())
            {
                if (Tank(target, ns->chase, ns->aoe))
                {
                    int engageLimit = DEFAULT_ENGAGE_LIMIT_DELAY;
                    if (commandVector.size() > 1)
                    {
                        std::string checkStr = commandVector.at(1);
                        engageLimit = atoi(checkStr.c_str());
                    }
                    ns->engageLimit = engageLimit;
                    std::ostringstream replyStream;
                    replyStream << "Try to engage " << target->GetName();
                    me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
                }
            }
        }
    }
    else if (commandName == "revive")
    {
        if (pmReceiver->IsAlive())
        {
            receiverAI->reviveDelay = 2000;
            if (Script_Base* sb = receiverAI->sb)
            {
                if (NingerMovement* rm = sb->rm)
                {
                    rm->ResetMovement();
                }
            }
        }
    }
    else if (commandName == "follow")
    {
        std::ostringstream replyStream;
        bool takeAction = true;
        if (commandVector.size() > 1)
        {
            std::string cmdDistanceStr = commandVector.at(1);
            float cmdDistance = atof(cmdDistanceStr.c_str());
            if (cmdDistance == 0.0f)
            {
                receiverAI->following = false;
                replyStream << "Stop following. ";
                takeAction = false;
            }
            else if (cmdDistance >= FOLLOW_MIN_DISTANCE && cmdDistance <= FOLLOW_MAX_DISTANCE)
            {
                receiverAI->followDistance = cmdDistance;
                replyStream << "Distance updated. ";
            }
            else
            {
                replyStream << "Distance is not valid. ";
                takeAction = false;
            }
        }
        if (takeAction)
        {
            receiverAI->eatDelay = 0;
            receiverAI->drinkDelay = 0;
            receiverAI->staying = false;
            receiverAI->holding = false;
            receiverAI->following = true;
            if (receiverAI->Follow())
            {
                replyStream << "Following " << receiverAI->followDistance;
            }
            else
            {
                replyStream << "can not follow";
            }
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "chase")
    {
        std::ostringstream replyStream;
        bool takeAction = true;
        if (commandVector.size() > 2)
        {
            std::string cmdMinDistanceStr = commandVector.at(1);
            float cmdMinDistance = atof(cmdMinDistanceStr.c_str());
            if (cmdMinDistance < MELEE_MIN_DISTANCE)
            {
                cmdMinDistance = MELEE_MIN_DISTANCE;
            }
            std::string cmdMaxDistanceStr = commandVector.at(2);
            float cmdMaxDistance = atof(cmdMaxDistanceStr.c_str());
            if (cmdMaxDistance > RANGE_HEAL_DISTANCE)
            {
                cmdMaxDistance = RANGE_HEAL_DISTANCE;
            }
            else if (cmdMaxDistance < MELEE_MAX_DISTANCE)
            {
                cmdMaxDistance = MELEE_MAX_DISTANCE;
            }
            if (cmdMinDistance > cmdMaxDistance)
            {
                cmdMinDistance = cmdMaxDistance - MELEE_MIN_DISTANCE;
            }
            receiverAI->chaseDistanceMin = cmdMinDistance;
            receiverAI->chaseDistanceMax = cmdMaxDistance;
            replyStream << "Chase distance range updated. " << receiverAI->chaseDistanceMin << " " << receiverAI->chaseDistanceMax;
        }
        else
        {
            replyStream << "Chase distance range is " << receiverAI->chaseDistanceMin << " " << receiverAI->chaseDistanceMax;
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "stay")
    {
        std::string targetGroupRole = "";
        if (commandVector.size() > 1)
        {
            targetGroupRole = commandVector.at(1);
        }
        if (receiverAI->Stay(targetGroupRole))
        {
            WhisperTo(pmSender, "Staying", Language::LANG_UNIVERSAL, pmReceiver);
        }
    }
    else if (commandName == "hold")
    {
        std::string targetGroupRole = "";
        if (commandVector.size() > 1)
        {
            targetGroupRole = commandVector.at(1);
        }
        if (receiverAI->Hold(targetGroupRole))
        {
            WhisperTo(pmReceiver, "Holding", Language::LANG_UNIVERSAL, pmSender);
        }
    }
    else if (commandName == "rest")
    {
        std::ostringstream replyStream;
        if (receiverAI->sb->Eat(true))
        {
            receiverAI->eatDelay = DEFAULT_REST_DELAY;
            receiverAI->drinkDelay = 1000;
            replyStream << "Resting";
        }
        else
        {
            replyStream << "Can not rest";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "who")
    {
        if (Awareness_Base* ningerAI = pmReceiver->awarenessMap[pmReceiver->activeAwarenessIndex])
        {
            if (Script_Base* sb = ningerAI->sb)
            {
                WhisperTo(pmSender, characterTalentTabNameMap[pmReceiver->getClass()][sb->maxTalentTab], Language::LANG_UNIVERSAL, pmReceiver);
            }
        }
    }
    else if (commandName == "assemble")
    {
        std::ostringstream replyStream;
        if (receiverAI->teleportAssembleDelay > 0)
        {
            replyStream << "I am on the way";
        }
        else
        {
            if (pmReceiver->IsAlive())
            {
                if (pmReceiver->GetDistance(pmSender) < VISIBILITY_DISTANCE_TINY)
                {
                    receiverAI->teleportAssembleDelay = urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS);
                    replyStream << "We are close, I will teleport to you in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
                }
                else
                {
                    receiverAI->teleportAssembleDelay = urand(30 * IN_MILLISECONDS, 1 * MINUTE * IN_MILLISECONDS);
                    replyStream << "I will teleport to you in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
                }
            }
            else
            {
                receiverAI->teleportAssembleDelay = urand(1 * MINUTE * IN_MILLISECONDS, 2 * MINUTE * IN_MILLISECONDS);
                replyStream << "I will teleport to you and revive in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
            }
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "gather")
    {
        std::ostringstream replyStream;
        if (receiverAI->moveDelay > 0)
        {
            replyStream << "I am on the way";
        }
        else
        {
            if (pmReceiver->IsAlive())
            {
                if (pmReceiver->GetDistance(pmSender) < RANGE_HEAL_DISTANCE)
                {
                    if (pmReceiver->IsNonMeleeSpellCast(false))
                    {
                        pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                        pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
                    }
                    pmReceiver->GetMotionMaster()->Initialize();
                    pmReceiver->StopMoving();
                    receiverAI->eatDelay = 0;
                    receiverAI->drinkDelay = 0;
                    int moveDelay = 1000;
                    if (commandVector.size() > 1)
                    {
                        std::string moveDelayStr = commandVector.at(1);
                        moveDelay = atoi(moveDelayStr.c_str());
                        if (moveDelay < 1000 || moveDelay > 6000)
                        {
                            moveDelay = 1000;
                        }
                    }
                    receiverAI->moveDelay = moveDelay;
                    receiverAI->sb->rm->MovePoint(pmSender->GetPosition(), moveDelay);
                    replyStream << "I will move to you";
                }
                else
                {
                    replyStream << "too far away";
                }
            }
            else
            {
                replyStream << "I am dead ";
            }
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "cast")
    {
        std::ostringstream replyStream;
        if (pmReceiver->IsAlive())
        {
            if (commandVector.size() > 1)
            {
                std::ostringstream targetStream;
                uint8 arrayCount = 0;
                for (std::vector<std::string>::iterator it = commandVector.begin(); it != commandVector.end(); it++)
                {
                    if (arrayCount > 0)
                    {
                        targetStream << (*it) << " ";
                    }
                    arrayCount++;
                }
                std::string spellName = TrimString(targetStream.str());
                Unit* senderTarget = pmSender->GetSelectedUnit();
                if (!senderTarget)
                {
                    senderTarget = pmReceiver;
                }
                if (receiverAI->sb->CastSpell(senderTarget, spellName, VISIBILITY_DISTANCE_NORMAL))
                {
                    replyStream << "Cast spell " << spellName << " on " << senderTarget->GetName();
                }
                else
                {
                    replyStream << "Can not cast spell " << spellName << " on " << senderTarget->GetName();
                }
            }
        }
        else
        {
            replyStream << "I am dead";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "cancel")
    {
        std::ostringstream replyStream;
        if (pmReceiver->IsAlive())
        {
            if (commandVector.size() > 1)
            {
                std::ostringstream targetStream;
                uint8 arrayCount = 0;
                for (std::vector<std::string>::iterator it = commandVector.begin(); it != commandVector.end(); it++)
                {
                    if (arrayCount > 0)
                    {
                        targetStream << (*it) << " ";
                    }
                    arrayCount++;
                }
                std::string spellName = TrimString(targetStream.str());
                if (receiverAI->sb->CancelAura(spellName))
                {
                    replyStream << "Aura canceled " << spellName;
                }
                else
                {
                    replyStream << "Can not cancel aura " << spellName;
                }
            }
        }
        else
        {
            replyStream << "I am dead";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "use")
    {
        std::ostringstream replyStream;
        if (pmReceiver->IsAlive())
        {
            if (commandVector.size() > 1)
            {
                std::string useType = commandVector.at(1);
                if (useType == "go")
                {
                    if (commandVector.size() > 2)
                    {
                        std::ostringstream goNameStream;
                        uint32 checkPos = 2;
                        while (checkPos < commandVector.size())
                        {
                            goNameStream << commandVector.at(checkPos) << " ";
                            checkPos++;
                        }
                        std::string goName = TrimString(goNameStream.str());
                        bool validToUse = false;
                        std::list<GameObject*> nearGOList;
                        pmReceiver->GetGameObjectListWithEntryInGrid(nearGOList, 0, MELEE_MAX_DISTANCE);
                        for (std::list<GameObject*>::iterator it = nearGOList.begin(); it != nearGOList.end(); it++)
                        {
                            if ((*it)->GetName() == goName)
                            {
                                pmReceiver->SetFacingToObject((*it));
                                pmReceiver->StopMoving();
                                pmReceiver->GetMotionMaster()->Initialize();
                                (*it)->Use(pmReceiver);
                                replyStream << "Use game object : " << goName;
                                validToUse = true;
                                break;
                            }
                        }
                        if (!validToUse)
                        {
                            replyStream << "No go with name " << goName << " nearby";
                        }
                    }
                    else
                    {
                        replyStream << "No go name";
                    }
                }
                else if (useType == "item")
                {

                }
                else
                {
                    replyStream << "Unknown type";
                }
            }
            else
            {
                replyStream << "Use what?";
            }
        }
        else
        {
            replyStream << "I am dead";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "stop")
    {
        std::ostringstream replyStream;
        if (pmReceiver->IsAlive())
        {
            pmReceiver->StopMoving();
            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_MELEE_SPELL);
            pmReceiver->AttackStop();
            if (Script_Base* sb = receiverAI->sb)
            {
                sb->PetStop();
                sb->ClearTarget();
            }
            receiverAI->moveDelay = 2000;
            replyStream << "Stopped";
        }
        else
        {
            replyStream << "I am dead";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "delay")
    {
        std::ostringstream replyStream;
        if (commandVector.size() > 1)
        {
            int delayMS = std::stoi(commandVector.at(1));
            receiverAI->dpsDelay = delayMS;
            replyStream << "DPS delay set to : " << delayMS;
        }
        else
        {
            replyStream << "DPS delay is : " << receiverAI->dpsDelay;
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "attackers")
    {
        std::ostringstream replyStream;
        if (pmReceiver->IsAlive())
        {
            replyStream << "attackers list : ";
            std::unordered_set<Unit*> attackers = pmReceiver->getAttackers();
            for (std::unordered_set<Unit*>::iterator aIT = attackers.begin(); aIT != attackers.end(); aIT++)
            {
                if (Unit* eachAttacker = *aIT)
                {
                    replyStream << eachAttacker->GetName() << ", ";
                }
            }
        }
        else
        {
            replyStream << "I am dead";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "cure")
    {
        std::ostringstream replyStream;
        if (commandVector.size() > 1)
        {
            std::string cureCMD = commandVector.at(1);
            if (cureCMD == "on")
            {
                receiverAI->cure = true;
            }
            else if (cureCMD == "off")
            {
                receiverAI->cure = false;
            }
            else
            {
                replyStream << "Unknown state";
            }
        }
        if (receiverAI->cure)
        {
            replyStream << "auto cure is on";
        }
        else
        {
            replyStream << "auto cure is off";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "petting")
    {
        std::ostringstream replyStream;
        if (commandVector.size() > 1)
        {
            std::string cureCMD = commandVector.at(1);
            if (cureCMD == "on")
            {
                receiverAI->petting = true;
            }
            else if (cureCMD == "off")
            {
                receiverAI->petting = false;
            }
            else
            {
                replyStream << "Unknown state";
            }
        }
        if (receiverAI->petting)
        {
            replyStream << "petting is on";
        }
        else
        {
            replyStream << "petting is off";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "aoe")
    {
        std::ostringstream replyStream;
        if (commandVector.size() > 1)
        {
            std::string on = commandVector.at(1);
            if (on == "on")
            {
                receiverAI->aoe = true;
            }
            else if (on == "off")
            {
                receiverAI->aoe = false;
            }
        }
        if (receiverAI->aoe)
        {
            replyStream << "AOE is on";
        }
        else
        {
            replyStream << "AOE is off";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "mark")
    {
        std::ostringstream replyStream;
        if (commandVector.size() > 1)
        {
            std::string markCMD = commandVector.at(1);
            if (markCMD == "on")
            {
                receiverAI->mark = true;
            }
            else if (markCMD == "off")
            {
                receiverAI->mark = false;
            }
            else
            {
                replyStream << "Unknown state";
            }
        }
        if (receiverAI->mark)
        {
            replyStream << "Mark is on";
        }
        else
        {
            replyStream << "Mark is off";
        }
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "emote")
    {
        if (pmReceiver->IsAlive())
        {
            if (commandVector.size() > 1)
            {
                int emoteCMD = std::stoi(commandVector.at(1));
                pmReceiver->HandleEmoteCommand((Emote)emoteCMD);
            }
            else
            {
                pmReceiver->AttackStop();
                pmReceiver->CombatStop();
            }
        }
        else
        {
            WhisperTo(pmSender, "I am dead", Language::LANG_UNIVERSAL, pmReceiver);
        }
    }
    else if (commandName == "pa")
    {
        if (pmReceiver->getClass() == Classes::CLASS_PALADIN)
        {
            std::ostringstream replyStream;
            if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
            {
                if (commandVector.size() > 1)
                {
                    std::string auratypeName = commandVector.at(1);
                    if (auratypeName == "concentration")
                    {
                        sp->auraType = PaladinAuraType::PaladinAuraType_Concentration;
                    }
                    else if (auratypeName == "devotion")
                    {
                        sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
                    }
                    else if (auratypeName == "retribution")
                    {
                        sp->auraType = PaladinAuraType::PaladinAuraType_Retribution;
                    }
                    else if (auratypeName == "fire")
                    {
                        sp->auraType = PaladinAuraType::PaladinAuraType_FireResistant;
                    }
                    else if (auratypeName == "frost")
                    {
                        sp->auraType = PaladinAuraType::PaladinAuraType_FrostResistant;
                    }
                    else if (auratypeName == "shadow")
                    {
                        sp->auraType = PaladinAuraType::PaladinAuraType_ShadowResistant;
                    }
                    else
                    {
                        replyStream << "Unknown type";
                    }
                }
                switch (sp->auraType)
                {
                case PaladinAuraType::PaladinAuraType_Concentration:
                {
                    replyStream << "concentration";
                    break;
                }
                case PaladinAuraType::PaladinAuraType_Devotion:
                {
                    replyStream << "devotion";
                    break;
                }
                case PaladinAuraType::PaladinAuraType_Retribution:
                {
                    replyStream << "retribution";
                    break;
                }
                case PaladinAuraType::PaladinAuraType_FireResistant:
                {
                    replyStream << "fire";
                    break;
                }
                case PaladinAuraType::PaladinAuraType_FrostResistant:
                {
                    replyStream << "frost";
                    break;
                }
                case PaladinAuraType::PaladinAuraType_ShadowResistant:
                {
                    replyStream << "shadow";
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
        }
    }
    else if (commandName == "pb")
    {
        if (pmReceiver->getClass() == Classes::CLASS_PALADIN)
        {
            std::ostringstream replyStream;
            if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
            {
                if (commandVector.size() > 1)
                {
                    std::string blessingTypeName = commandVector.at(1);
                    if (blessingTypeName == "kings")
                    {
                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Kings;
                    }
                    else if (blessingTypeName == "might")
                    {
                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
                    }
                    else if (blessingTypeName == "wisdom")
                    {
                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Wisdom;
                    }
                    else if (blessingTypeName == "salvation")
                    {
                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Salvation;
                    }
                    else
                    {
                        replyStream << "Unknown type";
                    }
                }
                switch (sp->blessingType)
                {
                case PaladinBlessingType::PaladinBlessingType_Kings:
                {
                    replyStream << "kings";
                    break;
                }
                case PaladinBlessingType::PaladinBlessingType_Might:
                {
                    replyStream << "might";
                    break;
                }
                case PaladinBlessingType::PaladinBlessingType_Wisdom:
                {
                    replyStream << "wisdom";
                    break;
                }
                case PaladinBlessingType::PaladinBlessingType_Salvation:
                {
                    replyStream << "salvation";
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
        }
    }
    else if (commandName == "ps")
    {
        if (pmReceiver->getClass() == Classes::CLASS_PALADIN)
        {
            std::ostringstream replyStream;
            if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
            {
                if (commandVector.size() > 1)
                {
                    std::string sealTypeName = commandVector.at(1);
                    if (sealTypeName == "righteousness")
                    {
                        sp->sealType = PaladinSealType::PaladinSealType_Righteousness;
                    }
                    else if (sealTypeName == "justice")
                    {
                        sp->sealType = PaladinSealType::PaladinSealType_Justice;
                    }
                    else if (sealTypeName == "command")
                    {
                        sp->sealType = PaladinSealType::PaladinSealType_Command;
                    }
                    else
                    {
                        replyStream << "Unknown type";
                    }
                }
                switch (sp->sealType)
                {
                case PaladinSealType::PaladinSealType_Righteousness:
                {
                    replyStream << "righteousness";
                    break;
                }
                case PaladinSealType::PaladinSealType_Justice:
                {
                    replyStream << "justice";
                    break;
                }
                case PaladinSealType::PaladinSealType_Command:
                {
                    replyStream << "command";
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
        }
    }
    else if (commandName == "ha")
    {
        if (pmReceiver->getClass() == Classes::CLASS_HUNTER)
        {
            std::ostringstream replyStream;
            if (Script_Hunter* sh = (Script_Hunter*)receiverAI->sb)
            {
                if (commandVector.size() > 1)
                {
                    std::string aspectName = commandVector.at(1);
                    if (aspectName == "hawk")
                    {
                        sh->aspectType = HunterAspectType::HunterAspectType_Hawk;
                    }
                    else if (aspectName == "monkey")
                    {
                        sh->aspectType = HunterAspectType::HunterAspectType_Monkey;
                    }
                    else if (aspectName == "wild")
                    {
                        sh->aspectType = HunterAspectType::HunterAspectType_Wild;
                    }
                    else if (aspectName == "pack")
                    {
                        sh->aspectType = HunterAspectType::HunterAspectType_Pack;
                    }
                    else
                    {
                        replyStream << "Unknown type";
                    }
                }
                switch (sh->aspectType)
                {
                case HunterAspectType::HunterAspectType_Hawk:
                {
                    replyStream << "hawk";
                    break;
                }
                case HunterAspectType::HunterAspectType_Monkey:
                {
                    replyStream << "monkey";
                    break;
                }
                case HunterAspectType::HunterAspectType_Wild:
                {
                    replyStream << "wild";
                    break;
                }
                case HunterAspectType::HunterAspectType_Pack:
                {
                    replyStream << "pack";
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
        }
    }
    else if (commandName == "equip")
    {
        if (commandVector.size() > 1)
        {
            std::string equipType = commandVector.at(1);
            if (equipType == "molten core")
            {
                if (pmReceiver->getClass() == Classes::CLASS_DRUID)
                {
                    for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
                    {
                        if (Item* currentEquip = pmReceiver->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
                        {
                            pmReceiver->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
                        }
                    }
                    EquipNewItem(pmReceiver, 16983, EquipmentSlots::EQUIPMENT_SLOT_HEAD);
                    EquipNewItem(pmReceiver, 19139, EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS);
                    EquipNewItem(pmReceiver, 16833, EquipmentSlots::EQUIPMENT_SLOT_CHEST);
                    EquipNewItem(pmReceiver, 11764, EquipmentSlots::EQUIPMENT_SLOT_WRISTS);
                    EquipNewItem(pmReceiver, 16831, EquipmentSlots::EQUIPMENT_SLOT_HANDS);
                    EquipNewItem(pmReceiver, 19149, EquipmentSlots::EQUIPMENT_SLOT_WAIST);
                    EquipNewItem(pmReceiver, 15054, EquipmentSlots::EQUIPMENT_SLOT_LEGS);
                    EquipNewItem(pmReceiver, 16982, EquipmentSlots::EQUIPMENT_SLOT_FEET);
                    EquipNewItem(pmReceiver, 18803, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND);
                    EquipNewItem(pmReceiver, 2802, EquipmentSlots::EQUIPMENT_SLOT_TRINKET1);
                    EquipNewItem(pmReceiver, 18406, EquipmentSlots::EQUIPMENT_SLOT_TRINKET2);
                    EquipNewItem(pmReceiver, 18398, EquipmentSlots::EQUIPMENT_SLOT_FINGER1);
                    EquipNewItem(pmReceiver, 18813, EquipmentSlots::EQUIPMENT_SLOT_FINGER2);
                    EquipNewItem(pmReceiver, 18811, EquipmentSlots::EQUIPMENT_SLOT_BACK);
                    EquipNewItem(pmReceiver, 16309, EquipmentSlots::EQUIPMENT_SLOT_NECK);
                    std::ostringstream replyStream;
                    replyStream << "Equip all fire resistance gears.";
                    me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
                }
            }
            else if (equipType == "reset")
            {
                InitializeEquipments(pmReceiver, true);
                std::ostringstream replyStream;
                replyStream << "All my equipments are reset.";
                me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
            }
        }
    }
    else if (commandName == "rti")
    {
        int targetIcon = -1;
        if (commandVector.size() > 1)
        {
            std::string iconIndex = commandVector.at(1);
            targetIcon = atoi(iconIndex.c_str());
        }
        if (targetIcon >= 0 && targetIcon < TARGETICONCOUNT)
        {
            receiverAI->sb->rti = targetIcon;
        }
        std::ostringstream replyStream;
        replyStream << "RTI is " << receiverAI->sb->rti;
        me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
    }
    else if (commandName == "assist")
    {
        if (receiverAI->sb->Assist(nullptr))
        {
            receiverAI->assistDelay = 5000;
            std::ostringstream replyStream;
            replyStream << "Try to pin down my RTI : " << receiverAI->sb->rti;
            me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
        }
    }
    else if (commandName == "prepare")
    {
        sNingerManager->PrepareNinger(pmReceiver);
    }
    else if (commandName == "wlc")
    {
        if (pmReceiver->getClass() == Classes::CLASS_WARLOCK)
        {
            std::ostringstream replyStream;
            if (Script_Warlock* swl = (Script_Warlock*)receiverAI->sb)
            {
                if (commandVector.size() > 1)
                {
                    std::string curseName = commandVector.at(1);
                    if (curseName == "none")
                    {
                        swl->curseType = WarlockCurseType::WarlockCurseType_None;
                    }
                    else if (curseName == "element")
                    {
                        swl->curseType = WarlockCurseType::WarlockCurseType_Element;
                    }
                    else if (curseName == "weakness")
                    {
                        swl->curseType = WarlockCurseType::WarlockCurseType_Weakness;
                    }
                    else if (curseName == "tongues")
                    {
                        swl->curseType = WarlockCurseType::WarlockCurseType_Tongues;
                    }
                    else
                    {
                        replyStream << "Unknown type";
                    }
                }
                switch (swl->curseType)
                {
                case WarlockCurseType::WarlockCurseType_None:
                {
                    replyStream << "none";
                    break;
                }
                case WarlockCurseType::WarlockCurseType_Element:
                {
                    replyStream << "element";
                    break;
                }
                case WarlockCurseType::WarlockCurseType_Weakness:
                {
                    replyStream << "weakness";
                    break;
                }
                case WarlockCurseType::WarlockCurseType_Tongues:
                {
                    replyStream << "tongues";
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            me->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmSender);
        }
    }
}

void NingerAction_Base::EquipRandomItem(uint32 pmEquipSlot, uint32 pmClass, uint32 pmSubclass, uint32 pmMinQuality, uint32 pmMaxRequiredLevel, uint32 pmModType)
{
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
        inventoryType = 20;
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
    else if (pmEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
    {
        inventoryType = 26;
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
    QueryResult itemQR = WorldDatabase.Query("SELECT entry FROM item_template where InventoryType = {} and class = {} and subclass = {} and Quality >= {} and RequiredLevel <= {} and RequiredLevel >= {} and AllowableClass = -1 and AllowableRace = -1 and RequiredSkill = 0 and requiredspell = 0 and requiredhonorrank = 0 and RequiredCityRank = 0 and RequiredReputationFaction = 0 order by rand()", inventoryType, pmClass, pmSubclass, pmMinQuality, pmMaxRequiredLevel, minRequiredLevel);
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
            for (uint32 statIndex = 0; statIndex < pProto->StatsCount; statIndex++)
            {
                if (pProto->ItemStat[statIndex].ItemStatType == pmModType)
                {                    
                    hasStat = true;
                    break;
                }
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

    if (me->IsNonMeleeSpellCast(true))
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

bool NingerAction_Base::CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura, bool pmOnlyMyAura, bool pmClearShapeShift)
{
    if (!me)
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false))
    {
        return true;
    }
    if (pmClearShapeShift)
    {
        ClearShapeshift();
    }
    if (!SpellValid(pmSpellId))
    {
        return false;
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
                    return false;
                }
            }
            else
            {
                if (pmTarget->HasAura(pmSpellId))
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
            rm->ResetMovement();
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
    else if (myLevel >= 60)
    {
        drinkEntry = 28399;
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
            rm->ResetMovement();
            return true;
        }
    }

    return false;
}

bool NingerAction_Base::Chase(Unit* pmTarget, float pmMinDistance, float pmMaxDistance)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (rm->Chase(pmTarget, pmMinDistance, pmMaxDistance))
    {
        ChooseTarget(pmTarget);
        return true;
    }
    return false;
}

bool NingerAction_Base::Follow(Unit* pmTarget, float pmMinDistance, float pmMaxDistance)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
    {
        return false;
    }
    else if (me->HasUnitState(UnitState::UNIT_STATE_NOT_MOVE))
    {
        return false;
    }
    else if (me->IsNonMeleeSpellCast(false))
    {
        return false;
    }
    return rm->Chase(pmTarget, pmMinDistance, pmMaxDistance);
}

bool NingerAction_Base::RandomTeleport()
{
    // todo : random teleport
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
