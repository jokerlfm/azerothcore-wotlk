#include "NingerAction_Rogue.h"
#include "Group.h"
#include "Spell.h"
#include "GridNotifiers.h"

NingerAction_Rogue::NingerAction_Rogue(Player* pmMe) :NingerAction_Base(pmMe)
{
    poisonStatus = 0;
    poisonDelay = 0;
    kickDelay = 0;
    sliceDelay = 0;
    adrenalineRushDelay = 0;
    dismantleDelay = 0;
    killingSpreeDelay = 0;
    evasionDelay = 0;
    sprintDelay = 0;
    revealingStrikeDelay = 0;
    recuperateDelay = 0;
}

void NingerAction_Rogue::Update(uint32 pmDiff)
{
	NingerAction_Base::Update(pmDiff);
    if (poisonDelay >= 0)
    {
        poisonDelay -= pmDiff;
    }
	if (kickDelay >= 0)
	{
		kickDelay -= pmDiff;
	}
    if (sliceDelay >= 0)
    {
        sliceDelay -= pmDiff;
    }
    if (adrenalineRushDelay >= 0)
    {
        adrenalineRushDelay -= pmDiff;
    }
    if (dismantleDelay >= 0)
    {
        dismantleDelay -= pmDiff;
    }
    if (killingSpreeDelay >= 0)
    {
        killingSpreeDelay -= pmDiff;
    }
    if (evasionDelay >= 0)
    {
        evasionDelay -= pmDiff;
    }
    if (sprintDelay >= 0)
    {
        sprintDelay -= pmDiff;
    }
    if (revealingStrikeDelay >= 0)
    {
        revealingStrikeDelay -= pmDiff;
    }
    if (recuperateDelay >= 0)
    {
        recuperateDelay -= pmDiff;
    }
}

void NingerAction_Rogue::Reset()
{
	NingerAction_Base::Reset();
}
/*
bool NingerAction_Rogue::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
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

    if (recuperateDelay < 0)
    {
        recuperateDelay = 1000;
        uint32 comboPoints = me->GetComboPoints();
        uint32 energy = me->GetPower(Powers::POWER_ENERGY);
        if (comboPoints > 0)
        {
            if (energy > 30)
            {
                if (Unit* comboTarget = ObjectAccessor::GetUnit(*me, me->GetComboTarget()))
                {
                    if (!comboTarget->IsAlive())
                    {
                        if (CastSpell(me, "Recuperate"))
                        {
                            recuperateDelay = comboPoints * 5 * TimeConstants::IN_MILLISECONDS;
                            return true;
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
		break;
	}
	case 1:
	{
		dpsResult = DPS_Combat(pmTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax);
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

	return dpsResult;
}

bool NingerAction_Rogue::DPS_Combat(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
	if (!me)
	{
		return false;
	}
	else if (!me->IsAlive())
	{
		return false;
	}

    uint32 comboPoints = me->GetComboPoints();
    uint32 energy = me->GetPower(Powers::POWER_ENERGY);

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
                    if (targetDistance > INTERACTION_DISTANCE)
                    {
                        if (sprintDelay < 0)
                        {
                            sprintDelay = 1000;
                            if (CastSpell(me, "Sprint"))
                            {
                                sprintDelay = 61000;
                                return true;
                            }
                        }
                    }
                    else
                    {
                        if (energy > 15)
                        {
                            if (kickDelay < 0)
                            {
                                kickDelay = 1000;
                                if (pmTarget->IsNonMeleeSpellCast(false))
                                {
                                    if (CastSpell(pmTarget, "Kick"))
                                    {
                                        kickDelay = 11000;
                                        return true;
                                    }
                                }
                            }
                        }
                        if (energy > 25)
                        {
                            if (dismantleDelay < 0)
                            {
                                dismantleDelay = 1000;
                                bool hasWeapon = true;
                                if (pmTarget->GetTypeId() == TYPEID_PLAYER)
                                {
                                    if (Player const* targetPlayer = pmTarget->ToPlayer())
                                    {
                                        if (!targetPlayer->GetWeaponForAttack(BASE_ATTACK) || !targetPlayer->IsUseEquipedWeapon(true))
                                        {
                                            hasWeapon = false;
                                        }
                                    }
                                }
                                else if (!pmTarget->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID))
                                {
                                    hasWeapon = false;
                                }
                                if (hasWeapon)
                                {
                                    if (CastSpell(pmTarget, "Dismantle"))
                                    {
                                        dismantleDelay = 61000;
                                        return true;
                                    }
                                }
                            }
                        }
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
                                if (targetsCount > 1)
                                {
                                    if (CastSpell(me, "Blade Flurry", true))
                                    {
                                        return true;
                                    }
                                }
                                else
                                {
                                    CancelAura("Blade Flurry");
                                }
                            }
                        }
                        if (killingSpreeDelay < 0)
                        {
                            killingSpreeDelay = 1000;
                            if (CastSpell(pmTarget, "Killing Spree"))
                            {
                                killingSpreeDelay = 121000;
                                return true;
                            }
                        }
                        if (evasionDelay < 0)
                        {
                            evasionDelay = 1000;
                            std::set<Unit*> const& myAttackers = me->getAttackers();
                            for (Unit* eachAttacker : myAttackers)
                            {
                                if (eachAttacker->GetTarget() == me->GetGUID())
                                {
                                    float attackerDistance = me->GetDistance(eachAttacker);
                                    if (attackerDistance < INTERACTION_DISTANCE)
                                    {
                                        if (CastSpell(me, "Evasion"))
                                        {
                                            evasionDelay = 181000;
                                            return true;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                        if (adrenalineRushDelay < 0)
                        {
                            adrenalineRushDelay = 1000;
                            if (CastSpell(me, "Adrenaline Rush"))
                            {
                                adrenalineRushDelay = 181000;
                                return true;
                            }
                        }
                        if (energy > 25)
                        {
                            if (sliceDelay < 0)
                            {
                                sliceDelay = 1000;
                                if (CastSpell(pmTarget, "Slice and Dice"))
                                {
                                    sliceDelay = 14000;
                                    return true;
                                }
                            }
                        }
                        if (energy > 35)
                        {
                            bool finish = false;
                            if (comboPoints >= 5)
                            {
                                finish = true;
                            }
                            else if (comboPoints >= 4)
                            {
                                if (urand(0, 100) < 80)
                                {
                                    finish = true;
                                }
                            }
                            else if (comboPoints >= 2)
                            {
                                if (urand(0, 100) < 50)
                                {
                                    finish = true;
                                }
                            }
                            if (finish)
                            {
                                if (CastSpell(pmTarget, "Eviscerate"))
                                {
                                    return true;
                                }
                            }
                        }
                        if (energy > 40)
                        {
                            if (revealingStrikeDelay < 0)
                            {
                                revealingStrikeDelay = 1000;
                                if (CastSpell(pmTarget, "Revealing Strike", true))
                                {
                                    revealingStrikeDelay = 10000;
                                    return true;
                                }
                            }
                            if (CastSpell(pmTarget, "Sinister Strike"))
                            {
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
					if (DPS_Combat(target, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
							if (DPS_Combat(tankTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
							if (DPS_Combat(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
							{
								return true;
							}
						}
					}
				}
                if (Unit* myTarget = me->GetSelectedUnit())
                {
                    if (DPS_Combat(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
				std::set<Unit*> const& myAttackers = me->getAttackers();
				for (Unit* eachAttacker : myAttackers)
				{
					if (DPS_Combat(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
				if (DPS_Combat(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
				{
					return true;
				}
			}
			std::set<Unit*> const& myAttackers = me->getAttackers();
			for (Unit* eachAttacker : myAttackers)
			{
				if (DPS_Combat(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool NingerAction_Rogue::Buff(Unit* pmTarget)
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
            if (recuperateDelay < 0)
            {
                recuperateDelay = 1000;
                uint32 comboPoints = me->GetComboPoints();
                uint32 energy = me->GetPower(Powers::POWER_ENERGY);
                if (comboPoints > 0)
                {
                    if (energy > 30)
                    {
                        if (Unit* comboTarget = ObjectAccessor::GetUnit(*me, me->GetComboTarget()))
                        {
                            if (!comboTarget->IsAlive())
                            {
                                if (CastSpell(me, "Recuperate"))
                                {
                                    recuperateDelay = comboPoints * 5 * TimeConstants::IN_MILLISECONDS;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
            if (poisonDelay < 0)
            {
                poisonDelay = 1000;
                uint32 maxInstancePoisonLevel = 0;
                for (std::unordered_map<uint32, uint32>::iterator ipIT = sNingerManager->instantPoisonEntryMap.begin(); ipIT != sNingerManager->instantPoisonEntryMap.end(); ipIT++)
                {
                    if (ipIT->first <= me->getLevel())
                    {
                        if (ipIT->first > maxInstancePoisonLevel)
                        {
                            maxInstancePoisonLevel = ipIT->first;
                        }
                    }
                }
                if (maxInstancePoisonLevel > 0)
                {
                    uint32 instancePoisonEntry = sNingerManager->instantPoisonEntryMap[maxInstancePoisonLevel];
                    if (!me->HasItemCount(instancePoisonEntry, 1))
                    {
                        me->StoreNewItemInBestSlots(instancePoisonEntry, 20);
                    }
                    Item* instancePoison = GetItemInInventory(instancePoisonEntry);
                    if (instancePoison && !instancePoison->IsInTrade())
                    {
                        if (poisonStatus == 0)
                        {
                            if (Item* weapon_mh = me->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND))
                            {
                                if (UseItem(instancePoison, weapon_mh))
                                {
                                    poisonStatus = 1;
                                    poisonDelay = 4000;
                                }
                            }
                        }
                        else if (poisonStatus == 1)
                        {
                            if (Item* weapon_oh = me->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND))
                            {
                                if (UseItem(instancePoison, weapon_oh))
                                {
                                    poisonStatus = 0;
                                    poisonDelay = 3601000;
                                }
                            }
                        }
                    }
                }
                else
                {
                    poisonDelay = 3601000;
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
