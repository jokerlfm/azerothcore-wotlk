#include "NingerAction_Warlock.h"
#include "Group.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "Pet.h"

NingerAction_Warlock::NingerAction_Warlock(Player* pmMe) :NingerAction_Base(pmMe)
{
    curseType = WarlockCurseType::WarlockCurseType_Weakness;
    curseDelay = 0;
    manaCheckDelay = 0;
    soulstoneDelay = 0;
    soulLinkDelay = 0;
    felArmorDelay = 0;
    wardDelay = 0;
    summonDelay = 0;
    soulHarvestDelay = 0;
    soulburnDelay = 0;
    soulFireDelay = 0;
    soulshatterDelay = 0;
    shadowfuryDelay = 0;
    conflagrateDelay = 0;
    immolateDelay = 0;
    shadowburnDelay = 0;
    chaosBoltDelay = 0;
    baneOfHavocDelay = 0;
    empoweredImpDelay = 0;
}

void NingerAction_Warlock::Update(uint32 pmDiff)
{
    NingerAction_Base::Update(pmDiff);
    if (curseDelay >= 0)
    {
        curseDelay -= pmDiff;
    }
    if (manaCheckDelay >= 0)
    {
        manaCheckDelay -= pmDiff;
    }
    if (soulstoneDelay >= 0)
    {
        soulstoneDelay -= pmDiff;
    }
    if (soulLinkDelay >= 0)
    {
        soulLinkDelay -= pmDiff;
    }
    if (felArmorDelay >= 0)
    {
        felArmorDelay -= pmDiff;
    }
    if (wardDelay >= 0)
    {
        wardDelay -= pmDiff;
    }
    if (summonDelay >= 0)
    {
        summonDelay -= pmDiff;
    }
    if (soulHarvestDelay >= 0)
    {
        soulHarvestDelay -= pmDiff;
    }
    if (soulburnDelay >= 0)
    {
        soulburnDelay -= pmDiff;
    }
    if (soulFireDelay >= 0)
    {
        soulFireDelay -= pmDiff;
    }
    if (soulshatterDelay >= 0)
    {
        soulshatterDelay -= pmDiff;
    }
    if (shadowfuryDelay >= 0)
    {
        shadowfuryDelay -= pmDiff;
    }
    if (conflagrateDelay >= 0)
    {
        conflagrateDelay -= pmDiff;
    }
    if (immolateDelay >= 0)
    {
        immolateDelay -= pmDiff;
    }
    if (shadowburnDelay >= 0)
    {
        shadowburnDelay -= pmDiff;
    }
    if (chaosBoltDelay >= 0)
    {
        chaosBoltDelay -= pmDiff;
    }
    if (baneOfHavocDelay >= 0)
    {
        baneOfHavocDelay -= pmDiff;
    }
    if (empoweredImpDelay >= 0)
    {
        empoweredImpDelay -= pmDiff;
    }
}

void NingerAction_Warlock::Reset()
{
    NingerAction_Base::Reset();
}
/*
bool NingerAction_Warlock::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    bool dpsResult = false;

    if (manaCheckDelay < 0)
    {
        manaCheckDelay = 1000;
        int manaPCT = me->GetPower(Powers::POWER_MANA) * 100 / me->GetMaxPower(Powers::POWER_MANA);
        if (manaPCT < 20)
        {
            float healthPCT = me->GetHealthPct();
            if (healthPCT > 50.0f)
            {
                if (CastSpell(me, "Life Tap"))
                {
                    return true;
                }
            }
        }
    }

    switch (maxTalentTab)
    {
    case 0:
    {
        break;
    }
    case 1:
    {
        break;
    }
    case 2:
    {
        dpsResult = DPS_Destruction(pmTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax);
        break;
    }
    default:
    {
        break;
    }
    }

    return dpsResult;
}

bool NingerAction_Warlock::DPS_Destruction(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
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
                                    if (summonDelay < 0)
                                    {
                                        summonDelay = 1000;
                                        if (CastSpell(pmTarget, "Summon Infernal"))
                                        {
                                            summonDelay = 601000;
                                            return true;
                                        }
                                    }
                                    if (shadowfuryDelay < 0)
                                    {
                                        shadowfuryDelay = 1000;
                                        if (CastSpell(pmTarget, "Shadowfury", true))
                                        {
                                            shadowfuryDelay = 21000;
                                            return true;
                                        }
                                    }
                                    if (CastSpell(pmTarget, "Rain of Fire"))
                                    {
                                        return true;
                                    }
                                }
                                else if (targetsCount == 2)
                                {
                                    if (baneOfHavocDelay < 0)
                                    {
                                        baneOfHavocDelay = 1000;
                                        for (std::list<Unit*>::iterator uIT = unitList.begin(); uIT != unitList.end(); uIT++)
                                        {
                                            if (Unit* eachUnit = *uIT)
                                            {
                                                if (me->IsValidAttackTarget(eachUnit))
                                                {
                                                    if (pmTarget->GetGUID() != eachUnit->GetGUID())
                                                    {
                                                        if (sNingerManager->HasAura(eachUnit, "Bane of Havoc", me))
                                                        {
                                                            baneOfHavocDelay = 10000;
                                                        }
                                                        else if (CastSpell(eachUnit, "Bane of Havoc"))
                                                        {
                                                            baneOfHavocDelay = 10000;
                                                            return true;
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
                        if (shadowburnDelay < 0)
                        {
                            shadowburnDelay = 1000;
                            float targetHealthPCT = pmTarget->GetHealthPct();
                            if (targetHealthPCT < 20.0f)
                            {
                                if (CastSpell(pmTarget, "Shadowburn"))
                                {
                                    shadowburnDelay = 15000;
                                    return true;
                                }
                            }
                        }
                        if (curseDelay < 0)
                        {
                            curseDelay = 1000;
                            switch (curseType)
                            {
                            case WarlockCurseType::WarlockCurseType_None:
                            {
                                curseDelay = 600000;
                                break;
                            }
                            case WarlockCurseType::WarlockCurseType_Element:
                            {
                                if (CastSpell(pmTarget, "Curse of the Elements", true))
                                {
                                    curseDelay = 5000;
                                    return true;
                                }
                                break;
                            }
                            case WarlockCurseType::WarlockCurseType_Tongues:
                            {
                                if (CastSpell(pmTarget, "Curse of Tongues", true))
                                {
                                    curseDelay = 5000;
                                    return true;
                                }
                                break;
                            }
                            case WarlockCurseType::WarlockCurseType_Weakness:
                            {
                                if (CastSpell(pmTarget, "Curse of Weakness", true))
                                {
                                    curseDelay = 5000;
                                    return true;
                                }
                                break;
                            }
                            default:
                            {
                                break;
                            }
                            }
                        }
                        int soulShards = me->GetPower(Powers::POWER_SOUL_SHARDS);
                        if (soulburnDelay < 0)
                        {
                            if (soulShards > 0)
                            {
                                soulburnDelay = 1000;
                                if (CastSpell(me, "Soulburn", true))
                                {
                                    soulburnDelay = 46000;
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            if (soulFireDelay < 0)
                            {
                                soulFireDelay = 1000;
                                if (sNingerManager->HasAura(me, "Soulburn"))
                                {
                                    if (CastSpell(pmTarget, "Soul Fire"))
                                    {
                                        return true;
                                    }
                                }
                            }
                        }
                        if (empoweredImpDelay < 0)
                        {
                            empoweredImpDelay = 1000;
                            if (me->HasAura(47283))
                            {
                                if (CastSpell(pmTarget, "Soul Fire"))
                                {
                                    empoweredImpDelay = 5000;
                                    return true;
                                }
                            }
                        }
                        if (immolateDelay < 0)
                        {
                            immolateDelay = 1000;
                            if (CastSpell(pmTarget, "Immolate", true, true))
                            {
                                return true;
                            }
                        }
                        if (conflagrateDelay < 0)
                        {
                            conflagrateDelay = 1000;
                            if (sNingerManager->HasAura(pmTarget, "Immolate", me))
                            {
                                if (CastSpell(pmTarget, "Conflagrate"))
                                {
                                    conflagrateDelay = 10000;
                                    return true;
                                }
                            }
                        }
                        if (chaosBoltDelay < 0)
                        {
                            chaosBoltDelay = 1000;
                            if (sNingerManager->HasAura(pmTarget, "Immolate", me))
                            {
                                if (CastSpell(pmTarget, "Chaos Bolt"))
                                {
                                    chaosBoltDelay = 12000;
                                    return true;
                                }
                            }
                        }
                        if (soulshatterDelay < 0)
                        {
                            soulshatterDelay = 1000;
                            if (CastSpell(me, "Soulshatter"))
                            {
                                soulshatterDelay = 121000;
                                return true;
                            }
                        }
                        if (CastSpell(pmTarget, "Incinerate"))
                        {
                            return true;
                        }
                        if (CastSpell(pmTarget, "Shadow Bolt"))
                        {
                            return true;
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
                    if (DPS_Destruction(target, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
                            if (DPS_Destruction(tankTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
                            if (DPS_Destruction(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                }
                if (Unit* myTarget = me->GetSelectedUnit())
                {
                    if (DPS_Destruction(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
                std::set<Unit*> const& myAttackers = me->getAttackers();
                for (Unit* eachAttacker : myAttackers)
                {
                    if (DPS_Destruction(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
                if (DPS_Destruction(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
            std::set<Unit*> const& myAttackers = me->getAttackers();
            for (Unit* eachAttacker : myAttackers)
            {
                if (DPS_Destruction(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool NingerAction_Warlock::Buff(Unit* pmTarget)
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
            int soulShards = me->GetPower(Powers::POWER_SOUL_SHARDS);
            if (Pet* myPet = me->GetPet())
            {
                if (soulLinkDelay < 0)
                {
                    soulLinkDelay = 1000;
                    float petDistance = me->GetDistance(myPet);
                    if (petDistance < FOLLOW_NORMAL_DISTANCE)
                    {
                        if (CastSpell(me, "Soul Link", true))
                        {
                            soulLinkDelay = 11000;
                            return true;
                        }
                    }
                }
            }
            else
            {
                if (soulShards > 0)
                {
                    if (soulburnDelay < 0)
                    {
                        soulburnDelay = 1000;
                        if (CastSpell(me, "Soulburn", true))
                        {
                            soulburnDelay = 46000;
                            return true;
                        }
                    }
                }
                if (CastSpell(me, "Summon Imp"))
                {
                    return true;
                }
            }
            if (felArmorDelay < 0)
            {
                felArmorDelay = 1000;
                if (CastSpell(me, "Fel Armor", true))
                {
                    felArmorDelay = 11000;
                    return true;
                }
            }
            if (wardDelay < 0)
            {
                wardDelay = 1000;
                if (me->HasAura(91713))
                {
                    me->CastSpell(me, 91711);
                    wardDelay = 31000;
                    return true;
                }
            }
            if (soulShards < 3)
            {
                if (soulHarvestDelay < 0)
                {
                    soulHarvestDelay = 1000;
                    if (CastSpell(me, "Soul Harvest"))
                    {
                        soulHarvestDelay = 31000;
                        return true;
                    }
                }
            }
            if (soulstoneDelay < 0)
            {
                soulstoneDelay = 10000;
                if (Group* myGroup = me->GetGroup())
                {
                    for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                    {
                        if (Player* member = groupRef->GetSource())
                        {
                            if (member->GetTypeId() == TypeID::TYPEID_PLAYER)
                            {
                                if (Player* targetPlayer = member->ToPlayer())
                                {
                                    if (Awareness_Base* ab = targetPlayer->awarenessMap[targetPlayer->activeAwarenessIndex])
                                    {
                                        if (ab->groupRole == GroupRole::GroupRole_Healer)
                                        {
                                            float targetDistance = me->GetDistance(targetPlayer);
                                            if (targetDistance < RANGE_DPS_DISTANCE)
                                            {
                                                if (!me->HasItemCount(5232, 1))
                                                {
                                                    if (CastSpell(me, "Create Soulstone"))
                                                    {
                                                        return true;
                                                    }
                                                }
                                                if (!sNingerManager->HasAura(targetPlayer, "Soulstone Resurrection"))
                                                {
                                                    if (Item* soulstone = GetItemInInventory(5232))
                                                    {
                                                        if (UseItem(soulstone, targetPlayer))
                                                        {
                                                            soulstoneDelay = 901000;
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
            return true;
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
