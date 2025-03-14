#include "NierAction_Priest.h"
#include "Player.h"
#include "Group.h"
#include "Spell.h"
#include "World.h"

NierAction_Priest::NierAction_Priest(Player* pmMe) :NierAction_Base(pmMe)
{
    spell_Shoot = 0;

    spell_CircleOfHealing = 0;
    spell_MindFlay = 0;
    spell_DesperatePrayer = 0;

    spell_Smite = 0;
    spell_Renew = 0;
    spell_LesserHeal = 0;
    spell_ShadowWord_Pain = 0;
    spell_MindBlast = 0;
    spell_InnerFire = 0;
    spell_ApolishDisease = 0;
    spell_HolyFire = 0;
    spell_ShadowProtection = 0;
    spell_ShackleUndead = 0;
    spell_MindSoothe = 0;
    spell_PsychicScream = 0;
    spell_Levitate = 0;
    spell_DevouringPlague = 0;
    spell_Fade = 0;
    spell_Shadowfiend = 0;
    spell_MindVision = 0;
    spell_ShadowWord_Death = 0;
    spell_MindControl = 0;
    spell_PrayerOfMending = 0;
    spell_FearWard = 0;
    spell_PrayerOfHealing = 0;
    spell_PrayerOfShadowProtection = 0;
    spell_PrayerOfFortitude = 0;
    spell_PrayerOfSpirit = 0;

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
    spell_Prayer_Of_Mending = 0;
    spell_GuardianSpirit = 0;
    aura_Surge_of_Light = 0;
}

void NierAction_Priest::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
{
    if (!me)
    {
        return;
    }
    if (me->IsInCombat() || !me->IsAlive())
    {
        return;
    }
    specialty = pmSpecialtyTabIndex;

    // holy priest only 
    specialty = 1;

    uint32 myLevel = me->getLevel();
    if (myLevel != pmTargetLevel)
    {
        PlayerSpellMap spellMap = me->GetSpellMap();
        for (PlayerSpellMap::const_iterator iter = spellMap.begin(); iter != spellMap.end(); ++iter)
        {
            me->removeSpell(iter->first, SPEC_MASK_ALL, false);
        }

        me->GiveLevel(pmTargetLevel);
        me->LearnDefaultSkills();
        me->LearnCustomSpells();
        me->learnQuestRewardedSpells();
        me->LearnDefaultSkills();
        me->learnQuestRewardedSpells();

        RemoveEquipments();
        myLevel = me->getLevel();
    }
    // stave 
    me->learnSpell(227);
    // wand
    me->learnSpell(5009, true);

    ResetTalent();
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
        spell_GuardianSpirit = 47788;
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
        spell_Prayer_Of_Mending = 33076;
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
        spell_Prayer_Of_Mending = 48112;
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
        spell_Prayer_Of_Mending = 48113;
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
    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
}

void NierAction_Priest::ResetTalent()
{
    if (!me)
    {
        return;
    }
    me->resetTalents(true);
    me->SendTalentsInfoData(false);

    // talent tab : 201 - discipline, 202 - holy, 203 - shadow

    switch (specialty)
    {
    case 0:
    {
        // discipline
        LearnTalent(1898);

        LearnTalent(344);
        LearnTalent(352);

        LearnTalent(348);
        LearnTalent(343);
        LearnTalent(347);

        LearnTalent(341);

        LearnTalent(351);
        LearnTalent(1201);

        LearnTalent(1771);
        LearnTalent(1772, 2);

        LearnTalent(322);
        LearnTalent(1772);
        LearnTalent(1773);

        LearnTalent(2235);
        LearnTalent(1896);

        LearnTalent(1774);
        LearnTalent(1894);
        LearnTalent(1901);

        LearnTalent(1202);

        LearnTalent(1897);
        LearnTalent(1895);

        LearnTalent(1858);

        LearnTalent(406);
        LearnTalent(401);

        LearnTalent(1181);

        LearnTalent(442);

        break;
    }
    case 1:
    {
        // holy
        LearnTalent(406);
        LearnTalent(401);
        LearnTalent(1181);
        LearnTalent(361);
        LearnTalent(408);
        LearnTalent(442);
        LearnTalent(1561);
        LearnTalent(402);
        LearnTalent(1766);
        LearnTalent(404);
        LearnTalent(1768);
        LearnTalent(2279);
        LearnTalent(1767);
        LearnTalent(1902);
        LearnTalent(1815);
        LearnTalent(1903);
        LearnTalent(1911);
        break;
    }
    case 2:
    {
        // shadow
        LearnTalent(462);
        LearnTalent(466);
        LearnTalent(482);
        LearnTalent(463);
        LearnTalent(501);
        LearnTalent(542);
        LearnTalent(461);
        LearnTalent(881);
        LearnTalent(541);
        LearnTalent(484);
        LearnTalent(1777);
        LearnTalent(1781);
        LearnTalent(521);
        LearnTalent(2267);
        LearnTalent(1778);
        LearnTalent(1816);
        LearnTalent(1779);
        LearnTalent(1908);
        LearnTalent(1909);
        LearnTalent(1907);
        LearnTalent(1910);

        break;
    }
    default:
    {
        break;
    }
    }

    me->SendTalentsInfoData(false);

    // priest trainer Astarii Starseeker
    TrainSpells(4090);
}

bool NierAction_Priest::InitializeEquipments(bool pmReset)
{
    if (!me)
    {
        return true;
    }
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
    uint32 myLevel = me->getLevel();
    uint32 minQuality = ItemQualities::ITEM_QUALITY_UNCOMMON;
    if (myLevel < 20)
    {
        minQuality = ItemQualities::ITEM_QUALITY_POOR;
    }
    for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
    {
        if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
        {
            if (myLevel < 30)
            {
                continue;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
        {
            if (myLevel < 30)
            {
                continue;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
        {
            if (myLevel < 30)
            {
                continue;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1 || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
        {
            if (myLevel < 30)
            {
                continue;
            }
        }
        uint32 modType = ItemModType::ITEM_MOD_INTELLECT;
        modType = -1;
        uint32 equipItemClass = 0;
        uint32 equipItemSubClass = 0;
        if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_BACK)
        {
            equipItemClass = 4;
            equipItemSubClass = 1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
        {
            equipItemClass = 4;
            equipItemSubClass = 0;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1)
        {
            equipItemClass = 4;
            equipItemSubClass = 0;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
        {
            equipItemClass = 4;
            equipItemSubClass = 0;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_MAINHAND)
        {
            equipItemClass = 2;
            equipItemSubClass = 10;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
        {
            equipItemClass = 2;
            equipItemSubClass = 19;
        }
        else
        {
            continue;
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
        EquipRandomItem(checkEquipSlot, equipItemClass, equipItemSubClass, minQuality, modType);
    }

    return true;
}

void NierAction_Priest::Prepare()
{
    NierAction_Base::Prepare();
    if (!me)
    {
        return;
    }
    else if (!me->IsAlive())
    {
        return;
    }

    me->Say("Prepared", Language::LANG_UNIVERSAL);
}

bool NierAction_Priest::DPS(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack)
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
    if (!pmTarget)
    {
        return false;
    }
    else if (!pmTarget->IsAlive())
    {
        if (me->GetTarget() == pmTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    else if (!me->IsValidAttackTarget(pmTarget))
    {
        if (me->GetTarget() == pmTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    if (!pmTarget->CanSeeOrDetect(me))
    {
        if (me->GetTarget() == pmTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    if (pmChasing)
    {
        if (!movement->Chase(pmTarget, pmDistanceMax, pmDistanceMin, pmHolding, pmForceBack))
        {
            if (me->GetTarget() == pmTarget->GetGUID())
            {
                ClearTarget();
            }
            return false;
        }
    }
    ChooseTarget(pmTarget);
    me->Attack(pmTarget, true);
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance < NIER_FAR_DISTANCE)
    {
        if (me->nierStrategyMap[me->activeStrategyIndex]->rushing)
        {
            // rushing 
        }
        else
        {
            // wand and pain
            if (spell_ShadowWord_Pain > 0)
            {
                if (CastSpell(pmTarget, spell_ShadowWord_Pain, true, true))
                {
                    return true;
                }
            }
            if (spell_DevouringPlague > 0)
            {
                if (CastSpell(pmTarget, spell_DevouringPlague, true, true))
                {
                    return true;
                }
            }
            bool shooting = false;
            if (Spell* spell = me->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL))
            {
                if (spell->m_targets.GetUnitTarget()->GetGUID() == pmTarget->GetGUID())
                {
                    shooting = true;
                }
                else
                {
                    me->InterruptSpell(CURRENT_AUTOREPEAT_SPELL, true);
                }
            }
            if (!shooting)
            {
                CastSpell(pmTarget, spell_Shoot);
            }
        }
    }

    return true;
}

bool NierAction_Priest::Heal(Unit* pmTarget, bool pmInstantOnly)
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
    if (!pmTarget)
    {
        return false;
    }
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return false;
    }
    if (me->GetPowerPct(Powers::POWER_MANA) < 30.0f)
    {
        ManaPotion();
    }
    switch (specialty)
    {
    case 0:
    {
        return Heal_Discipline(pmTarget, pmInstantOnly);
    }
    case 1:
    {
        return Heal_Holy(pmTarget, pmInstantOnly);
    }
    default:
    {
        break;
    }
    }

    return false;
}

bool NierAction_Priest::Heal_Discipline(Unit* pmTarget, bool pmInstantOnly)
{
    float targetHealthPct = pmTarget->GetHealthPct();
    if (targetHealthPct < 40.0f)
    {
        if (spell_DesperatePrayer > 0)
        {
            if (pmTarget->GetGUID() == me->GetGUID())
            {
                if (CastSpell(pmTarget, spell_DesperatePrayer))
                {
                    return true;
                }
            }
        }
        if (spell_PowerWord_Shield > 0)
        {
            if (!pmTarget->HasAura(spell_Weakened_Soul))
            {
                if (CastSpell(pmTarget, spell_PowerWord_Shield))
                {
                    return true;
                }
            }
        }
        if (spell_PainSuppression > 0)
        {
            if (CastSpell(pmTarget, spell_PainSuppression))
            {
                return true;
            }
        }
        if (spell_Penance > 0)
        {
            if (spell_InnerFocus > 0)
            {
                CastSpell(me, spell_InnerFocus);
            }
            if (CastSpell(pmTarget, spell_Penance))
            {
                return true;
            }
        }
        if (!pmInstantOnly)
        {
            if (spell_FlashHeal > 0)
            {
                if (CastSpell(pmTarget, spell_FlashHeal))
                {
                    return true;
                }
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
        if (spell_Prayer_Of_Mending > 0)
        {
            if (CastSpell(pmTarget, spell_Prayer_Of_Mending, true))
            {
                return true;
            }
        }
    }
    if (targetHealthPct < 70.0f)
    {
        if (spell_PowerInfusion > 0)
        {
            CastSpell(me, spell_PowerInfusion);
        }
        if (spell_PowerWord_Shield > 0)
        {
            if (!pmTarget->HasAura(spell_Weakened_Soul))
            {
                if (CastSpell(pmTarget, spell_PowerWord_Shield))
                {
                    return true;
                }
            }
        }
        if (spell_Penance > 0)
        {
            if (CastSpell(pmTarget, spell_Penance))
            {
                return true;
            }
        }
        if (pmInstantOnly)
        {
            if (me->GetDistance(pmTarget) < 10.0f)
            {
                if (spell_HolyNova > 0)
                {
                    if (CastSpell(me, spell_HolyNova))
                    {
                        return true;
                    }
                }
            }
        }
        else
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
    }

    return false;
}

bool NierAction_Priest::Heal_Holy(Unit* pmTarget, bool pmInstantOnly)
{
    float targetHealthPct = pmTarget->GetHealthPct();
    if (targetHealthPct < 90.0f)
    {
        if (aura_Surge_of_Light > 0)
        {
            if (me->HasAura(aura_Surge_of_Light))
            {
                if (CastSpell(pmTarget, spell_FlashHeal))
                {
                    return true;
                }
            }
        }
        if (targetHealthPct < 30.0f)
        {
            if (spell_PowerWord_Shield > 0)
            {
                if (!pmTarget->HasAura(spell_Weakened_Soul))
                {
                    if (CastSpell(pmTarget, spell_PowerWord_Shield))
                    {
                        return true;
                    }
                }
            }
            if (spell_GuardianSpirit > 0)
            {
                if (CastSpell(pmTarget, spell_GuardianSpirit))
                {
                    return true;
                }
            }
            if (!pmInstantOnly)
            {
                if (spell_FlashHeal > 0)
                {
                    if (CastSpell(pmTarget, spell_FlashHeal))
                    {
                        return true;
                    }
                }
            }
        }
        if (spell_Renew > 0)
        {
            if (CastSpell(pmTarget, spell_Renew, true, true))
            {
                return true;
            }
            if (spell_Prayer_Of_Mending > 0)
            {
                if (CastSpell(pmTarget, spell_Prayer_Of_Mending, true))
                {
                    return true;
                }
            }
        }
        if (targetHealthPct < 70.0f)
        {
            if (spell_PowerWord_Shield > 0)
            {
                if (!pmTarget->HasAura(spell_Weakened_Soul))
                {
                    if (CastSpell(pmTarget, spell_PowerWord_Shield))
                    {
                        return true;
                    }
                }
            }
            if (!pmInstantOnly)
            {
                if (spell_GreaterHeal > 0)
                {
                    if (spell_InnerFocus > 0)
                    {
                        CastSpell(me, spell_InnerFocus);
                    }
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
        }
    }

    return false;
}

bool NierAction_Priest::ReadyTank(Unit* pmTarget)
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
    if (!pmTarget)
    {
        return false;
    }
    else if (!pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return false;
    }

    if (spell_PowerWord_Shield > 0)
    {
        if (!pmTarget->HasAura(spell_Weakened_Soul))
        {
            if (CastSpell(pmTarget, spell_PowerWord_Shield))
            {
                return true;
            }
        }
    }

    if (spell_Prayer_Of_Mending > 0)
    {
        if (CastSpell(pmTarget, spell_Prayer_Of_Mending, true))
        {
            return true;
        }
    }

    return false;
}

bool NierAction_Priest::GroupHeal(Unit* pmTarget, bool pmInstantOnly)
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
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return false;
    }
    float targetHealthPct = pmTarget->GetHealthPct();
    if (targetHealthPct < 60.0f)
    {
        if (spell_Renew > 0)
        {
            if (CastSpell(pmTarget, spell_Renew, true, true))
            {
                return true;
            }
        }
        if (pmInstantOnly)
        {
            if (me->GetDistance(pmTarget) < 10.0f)
            {
                if (spell_HolyNova > 0)
                {
                    if (CastSpell(me, spell_HolyNova))
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            if (spell_Prayer_of_Healing > 0)
            {
                if (CastSpell(pmTarget, spell_Prayer_of_Healing))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool NierAction_Priest::SimpleHeal(Unit* pmTarget, bool pmInstantOnly)
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
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return false;
    }
    float targetHealthPct = pmTarget->GetHealthPct();
    if (targetHealthPct < 60.0f)
    {
        if (spell_Renew > 0)
        {
            if (CastSpell(pmTarget, spell_Renew, true, true))
            {
                return true;
            }
        }
        if (!pmInstantOnly)
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
    }

    return false;
}

bool NierAction_Priest::Cure(Unit* pmTarget)
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
    if (targetDistance > NIER_MAX_DISTANCE)
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
            if (aura->GetDuration() < 5 * IN_MILLISECONDS)
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
                    if (pST->GetDispelMask() & SpellInfo::GetDispelMask(DispelType::DISPEL_MAGIC))
                    {
                        if (CastSpell(pmTarget, spell_DispelMagic))
                        {
                            return true;
                        }
                    }
                }
                if (spell_CureDisease > 0)
                {
                    if (pST->GetDispelMask() & SpellInfo::GetDispelMask(DispelType::DISPEL_DISEASE))
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

bool NierAction_Priest::Buff(Unit* pmTarget)
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
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return false;
    }

    bool doBuff = true;
    if (spell_Prayer_of_Spirit > 0)
    {
        if (pmTarget->HasAura(spell_Prayer_of_Spirit))
        {
            doBuff = false;
        }
    }
    if (doBuff)
    {
        if (spell_DivineSpirit > 0)
        {
            if (pmTarget->HasAura(spell_DivineSpirit))
            {
                doBuff = false;
            }
        }
    }
    if (doBuff)
    {
        if (spell_Prayer_of_Spirit > 0)
        {
            if (CastSpell(pmTarget, spell_Prayer_of_Spirit))
            {
                return true;
            }
        }
        else if (spell_DivineSpirit > 0)
        {
            if (CastSpell(pmTarget, spell_DivineSpirit))
            {
                return true;
            }
        }
    }
    doBuff = true;
    if (spell_Prayer_of_Fortitude > 0)
    {
        if (pmTarget->HasAura(spell_Prayer_of_Fortitude))
        {
            doBuff = false;
        }
    }
    if (doBuff)
    {
        if (spell_PowerWord_Fortitude > 0)
        {
            if (pmTarget->HasAura(spell_PowerWord_Fortitude))
            {
                doBuff = false;
            }
        }
    }
    if (doBuff)
    {
        if (spell_Prayer_of_Fortitude > 0)
        {
            if (CastSpell(pmTarget, spell_Prayer_of_Fortitude))
            {
                return true;
            }
        }
        else if (spell_PowerWord_Fortitude > 0)
        {
            if (CastSpell(pmTarget, spell_PowerWord_Fortitude))
            {
                return true;
            }
        }
    }

    return false;
}

bool NierAction_Priest::Revive(Player* pmTarget)
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
    else if (pmTarget->IsAlive())
    {
        return false;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > NIER_MAX_DISTANCE)
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
