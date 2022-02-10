#include "NingerAction_Priest.h"
#include "Group.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"

NingerAction_Priest::NingerAction_Priest(Player* pmMe) :NingerAction_Base(pmMe)
{
    manaCheckDelay = 0;
    powerWordShieldDelay = 0;
    powerWordBarrierDelay = 0;
    fearWardDelay = 0;
    painSuppressionDelay = 0;
    penanceDelay = 0;
    innerFocusDelay = 0;
    powerInfusionDelay = 0;
    hymnOfHopeDelay = 0;
    prayerOfMendingDelay = 0;
    shadowfiendDelay = 0;
    mindBlastDelay = 0;
}

void NingerAction_Priest::LearnTalents(uint32 pmTabIndex)
{
    uint32 freePoints = me->GetFreeTalentPoints();
    if (freePoints > 0)
    {
        me->LearnTalent(1, freePoints >= 5 ? 5 : freePoints);
    }
    freePoints = me->GetFreeTalentPoints();
    if (freePoints > 0)
    {
        me->LearnTalent(1, freePoints >= 5 ? 5 : freePoints);
    }
}

void NingerAction_Priest::Update(uint32 pmDiff)
{
    NingerAction_Base::Update(pmDiff);
    if (manaCheckDelay >= 0)
    {
        manaCheckDelay -= pmDiff;
    }
    if (powerWordShieldDelay >= 0)
    {
        powerWordShieldDelay -= pmDiff;
    }
    if (powerWordBarrierDelay >= 0)
    {
        powerWordBarrierDelay -= pmDiff;
    }
    if (fearWardDelay >= 0)
    {
        fearWardDelay -= pmDiff;
    }
    if (painSuppressionDelay >= 0)
    {
        painSuppressionDelay -= pmDiff;
    }
    if (penanceDelay >= 0)
    {
        penanceDelay -= pmDiff;
    }
    if (innerFocusDelay >= 0)
    {
        innerFocusDelay -= pmDiff;
    }
    if (powerInfusionDelay >= 0)
    {
        powerInfusionDelay -= pmDiff;
    }
    if (hymnOfHopeDelay >= 0)
    {
        hymnOfHopeDelay -= pmDiff;
    }
    if (prayerOfMendingDelay >= 0)
    {
        prayerOfMendingDelay -= pmDiff;
    }
    if (shadowfiendDelay >= 0)
    {
        shadowfiendDelay -= pmDiff;
    }
    if (mindBlastDelay >= 0)
    {
        mindBlastDelay -= pmDiff;
    }
}

void NingerAction_Priest::Reset()
{
    NingerAction_Base::Reset();
}

/*
bool NingerAction_Priest::Revive(Player* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false))
    {
        return true;
    }
    if (pmTarget)
    {
        if (!pmTarget->IsAlive())
        {
            if (!pmTarget->IsResurrectRequested())
            {
                float targetDistance = me->GetDistance(pmTarget);
                if (targetDistance < RANGE_HEAL_DISTANCE)
                {
                    if (CastSpell(pmTarget, "Resurrection"))
                    {
                        return true;
                    }
                }
            }
        }
    }
    else
    {
        if (ogReviveTarget.IsEmpty())
        {
            if (Group* myGroup = me->GetGroup())
            {
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        if (Revive(member))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        else
        {
            Player* targetPlayer = ObjectAccessor::FindPlayer(ogReviveTarget);
            if (Revive(targetPlayer))
            {
                return true;
            }
        }
    }

    return true;
}

bool NingerAction_Priest::Cure(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (pmTarget->IsAlive())
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < RANGE_HEAL_DISTANCE)
            {
                for (uint32 type = SPELL_AURA_NONE; type < TOTAL_AURAS; ++type)
                {
                    std::list<AuraEffect*> auraList = pmTarget->GetAuraEffectsByType((AuraType)type);
                    for (auto auraIT = auraList.begin(), end = auraList.end(); auraIT != end; ++auraIT)
                    {
                        const SpellInfo* pST = (*auraIT)->GetSpellInfo();
                        if (!pST->IsPassive())
                        {
                            if (!pST->IsPositive())
                            {
                                if (pST->Dispel == DispelType::DISPEL_MAGIC)
                                {
                                    if (CastSpell(pmTarget, "Dispel Magic"))
                                    {
                                        return true;
                                    }
                                }
                                else if (pST->Dispel == DispelType::DISPEL_DISEASE)
                                {
                                    if (CastSpell(pmTarget, "Cure Disease"))
                                    {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (cureDelay < 0)
        {
            cureDelay = 1000;
            if (Group* myGroup = me->GetGroup())
            {
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        if (Cure(member))
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

bool NingerAction_Priest::Heal(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    bool healResult = false;

    if (manaCheckDelay < 0)
    {
        manaCheckDelay = 1000;
        int manaPCT = me->GetPower(Powers::POWER_MANA) * 100 / me->GetMaxPower(Powers::POWER_MANA);
        if (manaPCT < 20)
        {
            if (hymnOfHopeDelay < 0)
            {
                hymnOfHopeDelay = 1000;
                if (CastSpell(me, "Hymn of Hope"))
                {
                    hymnOfHopeDelay = 361000;
                    return true;
                }
            }
            if (shadowfiendDelay < 0)
            {
                shadowfiendDelay = 1000;
                if (Group* myGroup = me->GetGroup())
                {
                    for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                    {
                        if (Player* member = groupRef->GetSource())
                        {
                            if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                            {
                                if (ab->groupRole == GroupRole::GroupRole_Tank)
                                {
                                    if (Unit* tankTarget = member->GetSelectedUnit())
                                    {
                                        if (me->IsValidAttackTarget(tankTarget))
                                        {
                                            if (CastSpell(tankTarget, "Shadowfiend"))
                                            {
                                                shadowfiendDelay = 301000;
                                                return true;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    switch (maxTalentTab)
    {
    case 0:
    {
        healResult = Heal_Discipline(pmTarget);
        break;
    }
    case 1:
    {
        break;
    }
    case 2:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    return healResult;
}

bool NingerAction_Priest::Heal_Discipline(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (pmTarget->IsAlive())
        {
            float healthPCT = pmTarget->GetHealthPct();

            if (pmTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
            {
                if (Player* targetPlayer = pmTarget->ToPlayer())
                {
                    if (Awareness_Base* ab = targetPlayer->awarenessMap[targetPlayer->activeAwarenessIndex])
                    {
                        if (ab->groupRole == GroupRole::GroupRole_Tank)
                        {
                            if (pmTarget->IsInCombat())
                            {
                                if (powerWordShieldDelay < 0)
                                {
                                    powerWordShieldDelay = 1000;
                                    if (!sNingerManager->HasAura(pmTarget, "Weakened Soul"))
                                    {
                                        if (CastSpell(pmTarget, "Power Word: Shield"))
                                        {
                                            return true;
                                        }
                                    }
                                }
                            }
                            if (prayerOfMendingDelay < 0)
                            {
                                prayerOfMendingDelay = 1000;
                                if (CastSpell(pmTarget, "Prayer of Mending", true))
                                {
                                    prayerOfMendingDelay = 11000;
                                    return true;
                                }
                            }
                            if (healthPCT < 95.0f)
                            {
                                if (CastSpell(pmTarget, "Renew", true, true))
                                {
                                    return true;
                                }
                            }
                            if (healthPCT > 70.0f && healthPCT < 90.0f)
                            {
                                if (CastSpell(pmTarget, "Heal"))
                                {
                                    return true;
                                }
                            }
                            if (healthPCT < 70.0f)
                            {
                                if (penanceDelay < 0)
                                {
                                    penanceDelay = 1000;
                                    if (CastSpell(pmTarget, "Penance"))
                                    {
                                        penanceDelay = 13000;
                                        return true;
                                    }
                                }
                            }
                            if (pmTarget->IsInCombat())
                            {
                                if (healthPCT > 40.0f && healthPCT < 70.0f)
                                {
                                    if (powerInfusionDelay < 0)
                                    {
                                        powerInfusionDelay = 1000;
                                        if (!sNingerManager->HasAura(me, "Inner Focus"))
                                        {
                                            if (CastSpell(me, "Power Infusion"))
                                            {
                                                powerInfusionDelay = 121000;
                                            }
                                        }
                                    }
                                    else if (innerFocusDelay < 0)
                                    {
                                        innerFocusDelay = 1000;
                                        if (!sNingerManager->HasAura(me, "Power Infusion"))
                                        {
                                            if (CastSpell(me, "Inner Focus"))
                                            {
                                                innerFocusDelay = 46000;
                                            }
                                        }
                                    }
                                    if (CastSpell(pmTarget, "Greater Heal"))
                                    {
                                        return true;
                                    }
                                }
                                if (healthPCT < 40.0f)
                                {
                                    if (powerWordBarrierDelay < 0)
                                    {
                                        powerWordBarrierDelay = 1000;
                                        if (!sNingerManager->HasAura(pmTarget, "Pain Suppression"))
                                        {
                                            if (CastSpell(pmTarget, "Power Word: Barrier"))
                                            {
                                                powerWordBarrierDelay = 181000;
                                                return true;
                                            }
                                        }
                                    }
                                    else if (painSuppressionDelay < 0)
                                    {
                                        painSuppressionDelay = 1000;
                                        if (!sNingerManager->HasAura(pmTarget, "Power Word: Barrier"))
                                        {
                                            if (CastSpell(pmTarget, "Pain Suppression"))
                                            {
                                                painSuppressionDelay = 181000;
                                                return true;
                                            }
                                        }
                                    }
                                }
                                if (CastSpell(pmTarget, "Flash Heal"))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
            if (healthPCT < 80.0f)
            {
                if (CastSpell(pmTarget, "Renew", true, true))
                {
                    return true;
                }
                if (CastSpell(pmTarget, "Heal"))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        if (Group* myGroup = me->GetGroup())
        {
            Player* mainTank = NULL;
            for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                if (Player* member = groupRef->GetSource())
                {
                    if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                    {
                        if (ab->groupRole == GroupRole::GroupRole_Tank)
                        {
                            mainTank = member;
                            break;
                        }
                    }
                }
            }
            if (mainTank)
            {
                if (mainTank->IsAlive())
                {
                    if (Heal_Discipline(mainTank))
                    {
                        return true;
                    }
                }
            }
            for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                if (Player* member = groupRef->GetSource())
                {
                    if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                    {
                        if (ab->groupRole != GroupRole::GroupRole_Tank)
                        {
                            float healthPCT = member->GetHealthPct();
                            if (healthPCT < 50.0f)
                            {
                                if (Heal_Discipline(member))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool NingerAction_Priest::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }

    if (pmTarget)
    {
        if (pmTarget->IsAlive())
        {
            if (me->IsValidAttackTarget(pmTarget))
            {
                float targetDistance = me->GetDistance(pmTarget);
                if (targetDistance < VISIBILITY_DISTANCE_NORMAL)
                {
                    if (pmChase)
                    {
                        if (!Chase(pmTarget, pmChaseDistanceMin, pmChaseDistanceMax))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (!me->isInFront(pmTarget, M_PI / 4))
                        {
                            me->SetFacingToObject(pmTarget);
                        }
                    }
                    me->Attack(pmTarget, true);

                    if (targetDistance < RANGE_DPS_DISTANCE)
                    {
                        if (pmAOE)
                        {
                            if (aoeCheckDelay < 0)
                            {
                                aoeCheckDelay = 1000;
                                uint32 targetsCount = 0;
                                std::list<Unit*> unitList;
                                Trinity::AnyUnitInObjectRangeCheck go_check(me, AOE_TARGETS_RANGE);
                                Trinity::CreatureListSearcher<Trinity::AnyUnitInObjectRangeCheck> go_search(me, unitList, go_check);
                                Cell::VisitGridObjects(me, go_search, AOE_TARGETS_RANGE);
                                if (!unitList.empty())
                                {
                                    for (std::list<Unit*>::iterator uIT = unitList.begin(); uIT != unitList.end(); uIT++)
                                    {
                                        if (Unit* eachUnit = *uIT)
                                        {
                                            if (me->IsValidAttackTarget(eachUnit))
                                            {
                                                targetsCount++;
                                            }
                                        }
                                    }
                                }
                                if (targetsCount > 2)
                                {

                                }
                            }
                        }
                        if (mindBlastDelay < 0)
                        {
                            mindBlastDelay = 1000;
                            if (CastSpell(pmTarget, "Mind Blast"))
                            {
                                mindBlastDelay = 8000;
                                return true;
                            }
                        }
                    }
                    return true;
                }
            }
        }
    }
    else
    {
        if (Group* myGroup = me->GetGroup())
        {
            if (pmMark)
            {
                // icon  
                if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetGuidByTargetIcon(7)))
                {
                    if (DPS(target, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
            }
            else
            {
                // tank target
                Player* mainTank = NULL;
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                        {
                            if (ab->groupRole == GroupRole::GroupRole_Tank)
                            {
                                mainTank = member;
                                break;
                            }
                        }
                    }
                }
                if (mainTank)
                {
                    if (Unit* tankTarget = mainTank->GetSelectedUnit())
                    {
                        if (tankTarget->IsInCombat())
                        {
                            if (DPS(tankTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                    if (mainTank->IsAlive())
                    {
                        std::set<Unit*> const& tankAttackers = mainTank->getAttackers();
                        for (Unit* eachAttacker : tankAttackers)
                        {
                            if (DPS(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                }
                if (Unit* myTarget = me->GetSelectedUnit())
                {
                    if (DPS(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
                std::set<Unit*> const& myAttackers = me->getAttackers();
                for (Unit* eachAttacker : myAttackers)
                {
                    if (DPS(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            if (Unit* myTarget = me->GetSelectedUnit())
            {
                if (DPS(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
            std::set<Unit*> const& myAttackers = me->getAttackers();
            for (Unit* eachAttacker : myAttackers)
            {
                if (DPS(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool NingerAction_Priest::Buff(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (pmTarget->GetGUID() == me->GetGUID())
        {
            if (CastSpell(me, "Inner Fire", true))
            {
                return true;
            }
        }
        if (pmTarget->IsAlive())
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < RANGE_DPS_DISTANCE)
            {
                if (CastSpell(pmTarget, "Power Word: Fortitude", true))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        if (buffDelay < 0)
        {
            buffDelay = 2000;
            if (Group* myGroup = me->GetGroup())
            {
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        if (Buff(member))
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
*/
