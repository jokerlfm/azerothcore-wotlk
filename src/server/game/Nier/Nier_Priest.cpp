#include "Nier_Priest.h"
#include "WorldSessionMgr.h"

Nier_Priest::Nier_Priest()
{
    spell_Shoot = 0;
    spell_Smite = 0;

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
    spell_PowerWord_Shield = 0;
    spell_Penance = 0;
    spell_Weakened_Soul = 6788;
    spell_Prayer_of_Healing = 0;
    spell_Prayer_of_Spirit = 0;
    spell_Prayer_of_Fortitude = 0;
    spell_InnerFocus = 0;
    spell_PowerInfusion = 0;
    spell_PainSuppression = 0;
    spell_DesperatePrayer = 0;
    spell_HolyNova = 0;
    aura_Surge_of_Light = 0;
}

bool Nier_Priest::Attack(Unit* pTarget)
{
    if (!Nier_Base::Attack(pTarget))
    {
        return false;
    }

    return true;
}

bool Nier_Priest::Tank(Unit* pTarget)
{
    if (!Nier_Base::Tank(pTarget))
    {
        return false;
    }

    return true;
}

bool Nier_Priest::Heal(Unit* pTarget)
{
    if (!Nier_Base::Heal(pTarget))
    {
        return false;
    }

    float targetDistance = me->GetDistance(pTarget);
    if (targetDistance > VISIBILITY_DISTANCE_NORMAL)
    {
        return false;
    }

    ChooseTarget(pTarget);
    if (Chase(pTarget))
    {
        float targetHealthPct = pTarget->GetHealthPct();
        if (targetHealthPct < 90.0f)
        {
            if (targetHealthPct < 30.0f)
            {
                if (spell_PowerWord_Shield > 0)
                {
                    if (!pTarget->HasAura(spell_Weakened_Soul))
                    {
                        if (CastSpell(pTarget, spell_PowerWord_Shield))
                        {
                            return true;
                        }
                    }
                }
            }
            if (spell_Renew > 0)
            {
                if (CastSpell(pTarget, spell_Renew, true, true))
                {
                    return true;
                }
            }
            if (targetHealthPct < 70.0f)
            {
                if (spell_GreaterHeal > 0)
                {
                    if (spell_InnerFocus > 0)
                    {
                        CastSpell(me, spell_InnerFocus);
                    }
                    if (CastSpell(pTarget, spell_GreaterHeal))
                    {
                        return true;
                    }
                }
                if (spell_Heal > 0)
                {
                    if (CastSpell(pTarget, spell_Heal))
                    {
                        return true;
                    }
                }
                if (spell_LesserHeal > 0)
                {
                    if (CastSpell(pTarget, spell_LesserHeal))
                    {
                        return true;
                    }
                }
                if (spell_FlashHeal > 0)
                {
                    if (CastSpell(pTarget, spell_FlashHeal))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return true;
}

bool Nier_Priest::Cure(Unit* pTarget)
{
    if (!Nier_Base::Cure(pTarget))
    {
        return false;
    }

    return false;
}

bool Nier_Priest::Buff(Unit* pTarget)
{
    if (!Nier_Base::Buff(pTarget))
    {
        return false;
    }

    float targetDistance = me->GetDistance(pTarget);
    if (targetDistance > VISIBILITY_DISTANCE_TINY)
    {
        return false;
    }

    if (spell_DivineSpirit > 0)
    {
        if (CastSpell(pTarget, spell_DivineSpirit, true))
        {
            return true;
        }
    }
    if (spell_PowerWord_Fortitude > 0)
    {
        if (CastSpell(pTarget, spell_PowerWord_Fortitude, true))
        {
            return true;
        }
    }

    return false;
}

bool Nier_Priest::Revive(Unit* pTarget)
{
    if (!Nier_Base::Revive(pTarget))
    {
        return false;
    }

    ChooseTarget(pTarget);
    if (spell_Resurrection > 0)
    {
        if (CastSpell(pTarget, spell_Resurrection))
        {
            return true;
        }
    }

    return true;
}

bool Nier_Priest::InitializeCharacter(uint32 pTargetLevel)
{
    if (!Nier_Base::InitializeCharacter(pTargetLevel))
    {
        return false;
    }

    specialty = 0;
    me->ClearInCombat();
    uint32 myLevel = me->GetLevel();
    if (myLevel != pTargetLevel)
    {
        me->GiveLevel(pTargetLevel);
        me->LearnDefaultSkills();
        me->learnQuestRewardedSpells();

        ResetTalentsAndSpells();
        RemoveEquipments();
        myLevel = me->GetLevel();
    }
    // stave 
    me->learnSpell(227);
    // wand
    me->learnSpell(5009, true);
    spell_Shoot = 5019;
    spell_Smite = 585;

    spell_LesserHeal = 2050;
    spell_PowerWord_Fortitude = 1243;
    if (myLevel >= 4)
    {
        spell_LesserHeal = 2052;
    }
    if (myLevel >= 6)
    {
        spell_PowerWord_Shield = 17;
    }
    if (myLevel >= 8)
    {
        spell_Renew = 139;
    }
    if (myLevel >= 10)
    {
        spell_LesserHeal = 2053;
        spell_Resurrection = 2006;
    }
    if (myLevel >= 12)
    {
        spell_PowerWord_Fortitude = 1244;
        spell_PowerWord_Shield = 592;
    }
    if (myLevel >= 14)
    {
        spell_Renew = 6074;
        spell_CureDisease = 528;
    }
    if (myLevel >= 16)
    {
        spell_Heal = 2054;
    }
    if (myLevel >= 18)
    {
        spell_DispelMagic = 527;
        spell_PowerWord_Shield = 600;
    }
    if (myLevel >= 20)
    {
        spell_InnerFocus = 14751;
        spell_Renew = 6075;
        spell_FlashHeal = 2061;
        spell_HolyNova = 15237;
    }
    if (myLevel >= 22)
    {
        spell_Heal = 2055;
        spell_Resurrection = 2010;
    }
    if (myLevel >= 24)
    {
        spell_PowerWord_Fortitude = 1245;
        spell_PowerWord_Shield = 3747;
    }
    if (myLevel >= 26)
    {
        spell_Renew = 6076;
        spell_FlashHeal = 9472;
    }
    if (myLevel >= 28)
    {
        spell_Heal = 6063;
        spell_HolyNova = 15430;
    }
    if (myLevel >= 30)
    {
        spell_DivineSpirit = 14752;
        spell_PowerWord_Shield = 6065;
        spell_Prayer_of_Healing = 596;
    }
    if (myLevel >= 32)
    {
        spell_Renew = 6077;
        spell_FlashHeal = 9473;
    }
    if (myLevel >= 34)
    {
        spell_Heal = 6064;
        spell_Resurrection = 10880;
    }
    if (myLevel >= 36)
    {
        spell_PowerWord_Fortitude = 2791;
        spell_PowerWord_Shield = 6066;
        aura_Surge_of_Light = 33151;
        spell_HolyNova = 15431;
    }
    if (myLevel >= 38)
    {
        spell_Renew = 6078;
        spell_FlashHeal = 9474;
    }
    if (myLevel >= 40)
    {
        spell_GreaterHeal = 2060;
        spell_DivineSpirit = 14818;
        spell_Prayer_of_Healing = 996;
        spell_PowerInfusion = 10060;
    }
    if (myLevel >= 42)
    {
        spell_PowerWord_Shield = 10898;
    }
    if (myLevel >= 44)
    {
        spell_Renew = 10927;
        spell_FlashHeal = 10915;
        spell_HolyNova = 27799;
    }
    if (myLevel >= 46)
    {
        spell_GreaterHeal = 10963;
        spell_Resurrection = 10881;
    }
    if (myLevel >= 48)
    {
        spell_PowerWord_Fortitude = 10937;
        spell_PowerWord_Shield = 10899;
        spell_Prayer_of_Fortitude = 21562;
    }
    if (myLevel >= 50)
    {
        spell_Renew = 10928;
        spell_FlashHeal = 10916;
        spell_DivineSpirit = 14819;
        spell_Prayer_of_Healing = 10960;
        spell_PainSuppression = 33206;
    }
    if (myLevel >= 52)
    {
        spell_GreaterHeal = 10964;
        spell_HolyNova = 27800;
    }
    if (myLevel >= 54)
    {
        spell_PowerWord_Shield = 10900;
    }
    if (myLevel >= 56)
    {
        spell_Renew = 10929;
        spell_FlashHeal = 10917;
    }
    if (myLevel >= 58)
    {
        spell_GreaterHeal = 10965;
        spell_Resurrection = 20770;
    }
    if (myLevel >= 60)
    {
        spell_Renew = 25315;
        spell_GreaterHeal = 25314;
        spell_DivineSpirit = 27841;
        spell_PowerWord_Fortitude = 10938;
        spell_Penance = 47540;
        spell_PowerWord_Shield = 10901;
        spell_Prayer_of_Spirit = 27681;
        spell_Prayer_of_Fortitude = 21564;
        spell_Prayer_of_Healing = 10961;
        spell_HolyNova = 27801;
    }
    if (myLevel >= 61)
    {
        spell_FlashHeal = 25233;
    }
    if (myLevel >= 63)
    {
        spell_GreaterHeal = 25210;
    }
    if (myLevel >= 65)
    {
        spell_Renew = 25221;
        spell_PowerWord_Shield = 25217;
    }
    if (myLevel >= 67)
    {
        spell_FlashHeal = 25235;
    }
    if (myLevel >= 68)
    {
        spell_GreaterHeal = 25213;
        spell_Resurrection = 25435;
        spell_Prayer_of_Healing = 25308;
        spell_HolyNova = 25331;
    }
    if (myLevel >= 70)
    {
        spell_Renew = 25222;
        spell_DivineSpirit = 25312;
        spell_PowerWord_Fortitude = 25389;
        spell_Penance = 53005;
        spell_PowerWord_Shield = 25218;
        spell_Prayer_of_Spirit = 32999;
        spell_Prayer_of_Fortitude = 25392;
    }
    if (myLevel >= 73)
    {
        spell_GreaterHeal = 48062;
        spell_FlashHeal = 48070;
    }
    if (myLevel >= 74)
    {
        spell_InnerFocus = 14751;
    }
    if (myLevel >= 75)
    {
        spell_Renew = 48067;
        spell_Penance = 53006;
        spell_PowerWord_Shield = 48065;
        spell_HolyNova = 48007;
    }
    if (myLevel >= 76)
    {
        spell_Prayer_of_Healing = 48072;
    }
    if (myLevel >= 78)
    {
        spell_GreaterHeal = 48063;
        spell_Resurrection = 48171;
    }
    if (myLevel >= 79)
    {
        spell_FlashHeal = 48071;
    }
    if (myLevel >= 80)
    {
        spell_Renew = 48068;
        spell_DivineSpirit = 48073;
        spell_PowerWord_Fortitude = 48161;
        spell_Penance = 53007;
        spell_PowerWord_Shield = 48066;
        spell_Prayer_of_Spirit = 48074;
        spell_Prayer_of_Fortitude = 48162;
        spell_HolyNova = 48078;
    }
    me->UpdateSkillsToMaxSkillsForLevel();
    std::ostringstream msgStream;
    msgStream << me->GetName() << " initialized";
    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, msgStream.str());

    return true;
}

bool Nier_Priest::ResetTalentsAndSpells()
{
    if (!Nier_Base::ResetTalentsAndSpells())
    {
        return false;
    }

    // talent tab : 181 - Combat, 182 - Assassination
    LearnTalent(221);
    LearnTalent(181);
    LearnTalent(182);
    LearnTalent(1122);
    LearnTalent(223);
    LearnTalent(201);
    LearnTalent(1827);
    LearnTalent(1703);
    LearnTalent(186);
    LearnTalent(205);
    LearnTalent(1706);
    LearnTalent(206, 1);
    LearnTalent(1705);
    LearnTalent(1825);
    LearnTalent(1709);
    LearnTalent(2074);
    LearnTalent(2075);
    LearnTalent(2076);

    LearnTalent(276);
    LearnTalent(270);
    LearnTalent(273);
    LearnTalent(269);
    LearnTalent(682);

    me->SendTalentsInfoData(false);

    // rogue trainer Erion Shadewhisper
    TrainSpells(4214);

    return true;
}

void Nier_Priest::EquipRandomItem(uint32 pEquipSlot)
{
    uint32 itemClass = 0;
    uint32 itemSubclass = 0;
    uint32 inventoryType = 0;
    if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 1;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 3;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 9;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 6;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 8;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 10;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 5;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 7;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_BACK)
    {
        itemClass = 4;
        itemSubclass = 1;
        inventoryType = 16;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
    {
        itemClass = 4;
        itemSubclass = 0;
        inventoryType = 2;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1)
    {
        itemClass = 4;
        itemSubclass = 0;
        inventoryType = 11;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
    {
        itemClass = 4;
        itemSubclass = 0;
        inventoryType = 11;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_MAINHAND)
    {
        itemClass = 2;
        itemSubclass = 10;
        inventoryType = InventoryType::INVTYPE_2HWEAPON;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_OFFHAND)
    {
        inventoryType = 0;
    }
    else if (pEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
    {
        itemClass = 2;
        itemSubclass = 19;
        inventoryType = InventoryType::INVTYPE_RANGEDRIGHT;
    }
    else
    {
        return;
    }
    if (inventoryType == 0)
    {
        return;
    }
    int maxReqLevel = me->GetLevel();
    int minReqLevel = maxReqLevel - 10;
    if (minReqLevel < 0)
    {
        minReqLevel = 0;
    }

    if (Item* currentEquip = me->GetItemByPos(INVENTORY_SLOT_BAG_0, pEquipSlot))
    {
        if (currentEquip->GetTemplate()->RequiredLevel < minReqLevel)
        {
            me->DestroyItem(INVENTORY_SLOT_BAG_0, pEquipSlot, true);
        }
    }

    std::ostringstream msgStream;
    std::ostringstream itemQueryStream;
    itemQueryStream << "SELECT entry FROM item_template where class = " << itemClass << " and subclass = " << itemSubclass << " and InventoryType = " << inventoryType << " and RequiredLevel >= " << minReqLevel << " and RequiredLevel <= " << maxReqLevel << " and bonding < 4 and AllowableClass = -1 and AllowableRace = -1 and RequiredSkill = 0 and requiredspell = 0 and requiredhonorrank = 0 and RequiredCityRank = 0 and RequiredReputationFaction = 0 order by rand() ";
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
                if (me->CanEquipItem(pEquipSlot, dest, pItem, false) == InventoryResult::EQUIP_ERR_OK)
                {
                    me->EquipItem(dest, pItem, true);
                    msgStream << me->GetName() << " Equiped " << pItem->GetTemplate()->Name1;
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, msgStream.str());
                    return;
                }
            }
        } while (itemQR->NextRow());
    }
    msgStream << me->GetName() << " No usable equip " << pEquipSlot << " - " << minReqLevel << " - " << maxReqLevel;
    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, msgStream.str());
}
