#include "NingerAction_Priest.h"
#include "Group.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"

NingerAction_Priest::NingerAction_Priest() :NingerAction_Base()
{
    spell_Renew = 0;
    spell_LesserHeal = 0;
    spell_Heal = 0;
    spell_GreaterHeal = 0;
    spell_FlashHeal = 0;
    spell_Resurrection = 0;
    spell_CureDisease = 0;
    spell_DispelMagic = 0;
    spell_DivineSpirit = 0;
    spell_PowerWord_Fortitude = 0;
}

void NingerAction_Priest::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
{
    if (!me)
    {
        return;
    }
    me->ClearInCombat();
    if (me->getLevel() != pmTargetLevel)
    {
        me->GiveLevel(pmTargetLevel);
        me->LearnDefaultSkills();
        me->LearnCustomSpells();

        // todo : learn talents

        // priest trainer Astarii Starseeker
        TrainerSpellData const* trainer_spells = sObjectMgr->GetNpcTrainerSpells(4090);
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
        uint32 myLevel = me->getLevel();
        if (myLevel >= 80)
        {
            spell_Renew = 48068;
        }
        else if (myLevel >= 75)
        {
            spell_Renew = 48067;
        }
        else if (myLevel >= 70)
        {
            spell_Renew = 25222;
        }
        else if (myLevel >= 65)
        {
            spell_Renew = 25221;
        }
        else if (myLevel >= 60)
        {
            spell_Renew = 25315;
        }
        else if (myLevel >= 56)
        {
            spell_Renew = 10929;
        }
        else if (myLevel >= 50)
        {
            spell_Renew = 10928;
        }
        else if (myLevel >= 44)
        {
            spell_Renew = 10927;
        }
        else if (myLevel >= 38)
        {
            spell_Renew = 6078;
        }
        else if (myLevel >= 32)
        {
            spell_Renew = 6077;
        }
        else if (myLevel >= 26)
        {
            spell_Renew = 6076;
        }
        else if (myLevel >= 20)
        {
            spell_Renew = 6075;
        }
        else if (myLevel >= 14)
        {
            spell_Renew = 6074;
        }
        else if (myLevel >= 8)
        {
            spell_Renew = 139;
        }
        if (myLevel >= 10)
        {
            spell_LesserHeal = 2053;
        }
        else if (myLevel >= 4)
        {
            spell_LesserHeal = 2052;
        }
        else if (myLevel >= 1)
        {
            spell_LesserHeal = 2050;
        }
        if (myLevel >= 34)
        {
            spell_Heal = 6064;
        }
        else if (myLevel >= 28)
        {
            spell_Heal = 6063;
        }
        else if (myLevel >= 22)
        {
            spell_Heal = 2055;
        }
        else if (myLevel >= 16)
        {
            spell_Heal = 2054;
        }
        if (myLevel >= 78)
        {
            spell_GreaterHeal = 48063;
        }
        else if (myLevel >= 73)
        {
            spell_GreaterHeal = 48062;
        }
        else if (myLevel >= 68)
        {
            spell_GreaterHeal = 25213;
        }
        else if (myLevel >= 63)
        {
            spell_GreaterHeal = 25210;
        }
        else if (myLevel >= 60)
        {
            spell_GreaterHeal = 25314;
        }
        else if (myLevel >= 58)
        {
            spell_GreaterHeal = 10965;
        }
        else if (myLevel >= 52)
        {
            spell_GreaterHeal = 10964;
        }
        else if (myLevel >= 46)
        {
            spell_GreaterHeal = 10963;
        }
        else if (myLevel >= 40)
        {
            spell_GreaterHeal = 2060;
        }
        if (myLevel >= 79)
        {
            spell_FlashHeal = 48071;
        }
        else if (myLevel >= 73)
        {
            spell_FlashHeal = 48070;
        }
        else if (myLevel >= 67)
        {
            spell_FlashHeal = 25235;
        }
        else if (myLevel >= 61)
        {
            spell_FlashHeal = 25233;
        }
        else if (myLevel >= 56)
        {
            spell_FlashHeal = 10917;
        }
        else if (myLevel >= 50)
        {
            spell_FlashHeal = 10916;
        }
        else if (myLevel >= 44)
        {
            spell_FlashHeal = 10915;
        }
        else if (myLevel >= 38)
        {
            spell_FlashHeal = 9474;
        }
        else if (myLevel >= 32)
        {
            spell_FlashHeal = 9473;
        }
        else if (myLevel >= 26)
        {
            spell_FlashHeal = 9472;
        }
        else if (myLevel >= 20)
        {
            spell_FlashHeal = 2061;
        }
        if (myLevel >= 78)
        {
            spell_Resurrection = 48171;
        }
        else if (myLevel >= 68)
        {
            spell_Resurrection = 25435;
        }
        else if (myLevel >= 58)
        {
            spell_Resurrection = 20770;
        }
        else if (myLevel >= 46)
        {
            spell_Resurrection = 10881;
        }
        else if (myLevel >= 34)
        {
            spell_Resurrection = 10880;
        }
        else if (myLevel >= 22)
        {
            spell_Resurrection = 2010;
        }
        else if (myLevel >= 10)
        {
            spell_Resurrection = 2006;
        }
        if (myLevel >= 14)
        {
            spell_CureDisease = 528;
        }
        if (myLevel >= 18)
        {
            spell_DispelMagic = 527;
        }
        if (myLevel >= 80)
        {
            spell_DivineSpirit = 48073;
        }
        else if (myLevel >= 70)
        {
            spell_DivineSpirit = 25312;
        }
        else if (myLevel >= 60)
        {
            spell_DivineSpirit = 27841;
        }
        else if (myLevel >= 50)
        {
            spell_DivineSpirit = 14819;
        }
        else if (myLevel >= 40)
        {
            spell_DivineSpirit = 14818;
        }
        else if (myLevel >= 30)
        {
            spell_DivineSpirit = 14752;
        }
        if (myLevel >= 80)
        {
            spell_PowerWord_Fortitude = 48161;
        }
        else if (myLevel >= 70)
        {
            spell_PowerWord_Fortitude = 25389;
        }
        else if (myLevel >= 60)
        {
            spell_PowerWord_Fortitude = 10938;
        }
        else if (myLevel >= 48)
        {
            spell_PowerWord_Fortitude = 10937;
        }
        else if (myLevel >= 36)
        {
            spell_PowerWord_Fortitude = 2791;
        }
        else if (myLevel >= 24)
        {
            spell_PowerWord_Fortitude = 1245;
        }
        else if (myLevel >= 12)
        {
            spell_PowerWord_Fortitude = 1244;
        }
        else if (myLevel >= 1)
        {
            spell_PowerWord_Fortitude = 1243;
        }
        // todo : initial equipments

        InitializeEquipments(true);
    }
    std::ostringstream msgStream;
    msgStream << me->GetName() << " initialized";
    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
}

void NingerAction_Priest::InitializeEquipments(bool pmReset)
{
    if (pmReset)
    {
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
    uint32 minQuality = ItemQualities::ITEM_QUALITY_UNCOMMON;
    if (me->getLevel() < 20)
    {
        minQuality = ItemQualities::ITEM_QUALITY_POOR;
    }
    for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
    {
        if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
        {
            if (me->getLevel() < 30)
            {
                continue;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
        {
            if (me->getLevel() < 30)
            {
                continue;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
        {
            if (me->getLevel() < 30)
            {
                continue;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1 || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
        {
            if (me->getLevel() < 30)
            {
                continue;
            }
        }
        if (Item* currentEquip = me->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
        {
            if (const ItemTemplate* checkIT = currentEquip->GetTemplate())
            {
                if (checkIT->Quality >= minQuality)
                {
                    continue;
                }
                else
                {
                    me->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
                }
            }
        }
        // todo : random pick and equip 
    }
}

bool NingerAction_Priest::Heal(Unit* pmTarget)
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
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > RANGE_MAX_DISTANCE)
    {
        return false;
    }
    float targetHealthPct = pmTarget->GetHealthPct();
    if (targetHealthPct < 30.0f)
    {
        if (spell_FlashHeal > 0)
        {
            if (CastSpell(pmTarget, spell_FlashHeal))
            {
                return true;
            }
        }
    }
    if (targetHealthPct < 70.0f)
    {
        if (spell_GreaterHeal > 0)
        {
            if (CastSpell(pmTarget, spell_GreaterHeal))
            {
                return true;
            }
        }
        if (spell_Heal > 0)
        {
            if (CastSpell(pmTarget, spell_Heal))
            {
                return true;
            }
        }
        if (spell_LesserHeal > 0)
        {
            if (CastSpell(pmTarget, spell_LesserHeal))
            {
                return true;
            }
        }
    }
    if (targetHealthPct < 90.0f)
    {
        if (spell_Renew > 0)
        {
            if (CastSpell(pmTarget, spell_Renew, true, true))
            {
                return true;
            }
        }
    }

    return false;
}

bool NingerAction_Priest::SimpleHeal(Unit* pmTarget)
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
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > RANGE_MAX_DISTANCE)
    {
        return false;
    }
    float targetHealthPct = pmTarget->GetHealthPct();
    if (targetHealthPct < 50.0f)
    {
        if (spell_GreaterHeal > 0)
        {
            if (CastSpell(pmTarget, spell_GreaterHeal))
            {
                return true;
            }
        }
        if (spell_Heal > 0)
        {
            if (CastSpell(pmTarget, spell_Heal))
            {
                return true;
            }
        }
        if (spell_LesserHeal > 0)
        {
            if (CastSpell(pmTarget, spell_LesserHeal))
            {
                return true;
            }
        }
    }

    return false;
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
    if (!pmTarget)
    {
        return false;
    }
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > RANGE_MAX_DISTANCE)
    {
        return false;
    }

    Unit::VisibleAuraMap const* visibleAuras = pmTarget->GetVisibleAuras();
    for (Unit::VisibleAuraMap::const_iterator itr = visibleAuras->begin(); itr != visibleAuras->end(); ++itr)
    {
        if (Aura* aura = itr->second->GetBase())
        {
            if (aura->IsPassive())
            {
                continue;
            }
            if (aura->GetDuration() < 10 * IN_MILLISECONDS)
            {
                continue;
            }
            if (const SpellInfo* pST = aura->GetSpellInfo())
            {
                if (pST->IsPositive())
                {
                    continue;
                }
                if (spell_DispelMagic > 0)
                {
                    if (pST->GetDispelMask() & DISPEL_MAGIC)
                    {
                        if (CastSpell(pmTarget, spell_DispelMagic))
                        {
                            return true;
                        }
                    }
                }
                if (spell_CureDisease > 0)
                {
                    if (pST->GetDispelMask() & DispelType::DISPEL_DISEASE)
                    {
                        if (CastSpell(pmTarget, spell_CureDisease))
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
    if (!pmTarget)
    {
        return false;
    }
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > RANGE_MAX_DISTANCE)
    {
        return false;
    }

    if (spell_DivineSpirit > 0)
    {
        if (CastSpell(pmTarget, spell_DivineSpirit, true))
        {
            return true;
        }
    }
    if (spell_PowerWord_Fortitude > 0)
    {
        if (CastSpell(pmTarget, spell_PowerWord_Fortitude, true))
        {
            return true;
        }
    }

    return false;
}

bool NingerAction_Priest::Assist(Unit* pmTarget)
{
    return false;
}

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
    if (!pmTarget)
    {
        return false;
    }
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > RANGE_MAX_DISTANCE)
    {
        return false;
    }

    if (spell_Resurrection > 0)
    {
        if (CastSpell(pmTarget, spell_Resurrection))
        {
            return true;
        }
    }

    return false;
}
