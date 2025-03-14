#include "NierAction_Hunter.h"
#include "World.h"
#include "Pet.h"

NierAction_Hunter::NierAction_Hunter(Player* pmMe) :NierAction_Base(pmMe)
{
    specialty = 0;

    aspectType = HunterAspectType::HunterAspectType_Hawk;

    spell_ArcaneShot = 0;
    spell_SerpentSting = 0;
    spell_ScorpidSting = 0;
    spell_RapidFire = 0;
    spell_FeignDeath = 0;
    spell_HuntersMark = 0;
    spell_Volley = 0;
    spell_SteadyShot = 0;
    spell_AspectOfTheHawk = 0;
    spell_AspectOfTheViper = 0;
    spell_CallPet = 0;
    spell_DismissPet = 0;
    spell_RevivePet = 0;
    spell_TameBeast = 0;
    spell_MendPet = 0;
    spell_AspectOfTheDragonhawk = 0;
    spell_ConcussiveShot = 0;
    spell_AutoShot = 0;
    spell_Readiness = 0;

    spell_ScatterShot = 0;
    spell_TrueshotAura = 0;
    spell_SilencingShot = 0;
    spell_ChimeraShot = 0;

    ammoEntry = 0;
}

void NierAction_Hunter::InitializeCharacter(uint32 pmTarGetLevel, uint32 pmSpecialtyTabIndex)
{
    if (!me)
    {
        return;
    }
    specialty = pmSpecialtyTabIndex;

    // Marksmanship hunter only 
    specialty = 1;

    me->ClearInCombat();
    uint32 myLevel = me->getLevel();
    if (myLevel != pmTarGetLevel)
    {
        me->GiveLevel(pmTarGetLevel);
        me->LearnDefaultSkills();
        me->learnQuestRewardedSpells();

        ResetTalent();
        RemoveEquipments();
        myLevel = me->getLevel();
    }
    spell_AutoShot = 75;
    me->learnSpell(197, true);
    me->learnSpell(202, true);
    me->learnSpell(264, true);
    me->learnSpell(266, true);
    me->learnSpell(5011, true);
    ammoEntry = 2512;
    if (myLevel >= 4)
    {
        spell_SerpentSting = 1978;
    }
    if (myLevel >= 6)
    {
        spell_ArcaneShot = 3044;
        spell_HuntersMark = 1130;
    }
    if (myLevel >= 8)
    {
        spell_ConcussiveShot = 5116;
    }
    if (myLevel >= 10)
    {
        ammoEntry = 2515;
        spell_SerpentSting = 13549;
        spell_AspectOfTheHawk = 13165;
        me->CastSpell(me, 23356, TriggerCastFlags::TRIGGERED_NONE);
        me->CastSpell(me, 23357, TriggerCastFlags::TRIGGERED_NONE);
        spell_CallPet = 883;
        spell_DismissPet = 2641;
        spell_RevivePet = 982;
        spell_TameBeast = 1515;
    }
    if (myLevel >= 12)
    {
        spell_ArcaneShot = 14281;
        spell_MendPet = 136;
    }
    if (myLevel >= 18)
    {
        spell_SerpentSting = 13550;
        spell_AspectOfTheHawk = 14318;
    }
    if (myLevel >= 20)
    {
        spell_ArcaneShot = 14282;
        spell_MendPet = 3111;
        spell_AspectOfTheViper = 34074;
        me->learnSpell(200, true);
    }
    if (myLevel >= 22)
    {
        spell_ScorpidSting = 3043;
        spell_HuntersMark = 14323;
    }
    if (myLevel >= 26)
    {
        ammoEntry = 3030;
    }
    if (myLevel >= 26)
    {
        spell_SerpentSting = 13551;
        spell_RapidFire = 3045;
    }
    if (myLevel >= 28)
    {
        spell_ArcaneShot = 14283;
        spell_AspectOfTheHawk = 14319;
        spell_MendPet = 3661;
    }
    if (myLevel >= 30)
    {
        spell_FeignDeath = 5384;
    }
    if (myLevel >= 34)
    {
        spell_SerpentSting = 13552;
    }
    if (myLevel >= 36)
    {
        spell_ArcaneShot = 14284;
        spell_MendPet = 3662;
    }
    if (myLevel >= 37)
    {
        spell_Readiness = 23989;
    }
    if (myLevel >= 38)
    {
        spell_AspectOfTheHawk = 14320;
    }
    if (myLevel >= 40)
    {
        ammoEntry = 11285;
        spell_HuntersMark = 14324;
        spell_Volley = 1510;
    }
    if (myLevel >= 41)
    {
        spell_TrueshotAura = 19506;
    }
    if (myLevel >= 42)
    {
        spell_SerpentSting = 13553;
    }
    if (myLevel >= 44)
    {
        spell_ArcaneShot = 14285;
        spell_MendPet = 13542;
    }
    if (myLevel >= 48)
    {
        spell_AspectOfTheHawk = 14321;
    }
    if (myLevel >= 50)
    {
        spell_SerpentSting = 13554;
        spell_Volley = 14294;
        spell_SteadyShot = 56641;
    }
    if (myLevel >= 52)
    {
        spell_ArcaneShot = 14286;
        spell_MendPet = 13543;
        spell_SilencingShot = 34490;
    }
    if (myLevel >= 55)
    {
        ammoEntry = 28053;
    }
    if (myLevel >= 58)
    {
        spell_SerpentSting = 13555;
        spell_HuntersMark = 14325;
        spell_Volley = 14295;
        spell_AspectOfTheHawk = 14322;
    }
    if (myLevel >= 60)
    {
        spell_ArcaneShot = 14287;
        spell_SerpentSting = 25295;
        spell_AspectOfTheHawk = 25296;
        spell_MendPet = 13544;
        spell_ChimeraShot = 53209;
    }
    if (myLevel >= 62)
    {
        spell_SteadyShot = 34120;
    }
    if (myLevel >= 65)
    {
        ammoEntry = 28056;
    }
    if (myLevel >= 67)
    {
        spell_SerpentSting = 27016;
        spell_Volley = 27022;
    }
    if (myLevel >= 68)
    {
        spell_AspectOfTheHawk = 27044;
        spell_MendPet = 27046;
    }
    if (myLevel >= 69)
    {
        spell_ArcaneShot = 27019;
    }
    if (myLevel >= 71)
    {
        spell_SteadyShot = 49051;
    }
    if (myLevel >= 73)
    {
        spell_ArcaneShot = 49044;
        spell_SerpentSting = 49000;
    }
    if (myLevel >= 74)
    {
        spell_Volley = 58431;
        spell_AspectOfTheDragonhawk = 61846;
        spell_MendPet = 48989;
    }
    if (myLevel >= 75)
    {
        ammoEntry = 41586;
    }
    if (myLevel >= 76)
    {
        spell_HuntersMark = 53338;
    }
    if (myLevel >= 77)
    {
        spell_SteadyShot = 49052;
    }
    if (myLevel >= 79)
    {
        spell_ArcaneShot = 49045;
        spell_SerpentSting = 49001;
    }
    if (myLevel >= 80)
    {
        ammoEntry = 52021;
        spell_Volley = 58434;
        spell_AspectOfTheDragonhawk = 61847;
        spell_MendPet = 48990;
    }
    me->UpdateSkillsToMaxSkillsForLevel();
    std::ostringstream msgStream;
    msgStream << me->GetName() << " initialized";
    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
}

void NierAction_Hunter::ResetTalent()
{
    if (!me)
    {
        return;
    }
    me->resetTalents(true);
    me->SendTalentsInfoData(false);

    // talent tab : 361 - Beast Mastery, 363 - Marksmanship
    LearnTalent(2197);
    LearnTalent(1344);
    LearnTalent(1349);
    LearnTalent(1343);
    LearnTalent(1346);
    LearnTalent(1348);
    LearnTalent(1342);
    LearnTalent(1353);
    LearnTalent(1362);
    LearnTalent(1361);
    LearnTalent(1804);
    LearnTalent(1806);
    LearnTalent(1807);
    LearnTalent(1808);
    LearnTalent(2132);
    LearnTalent(2134);
    LearnTalent(2135);
    LearnTalent(2132, 3);
    LearnTalent(2130);
    LearnTalent(2133);
    LearnTalent(1818);

    LearnTalent(1382);
    LearnTalent(1624);
    LearnTalent(1395);
    LearnTalent(2138);

    me->SendTalentsInfoData(false);

    // hunter trainer Nightrunner
    TrainSpells(4138);
}

bool NierAction_Hunter::InitializeEquipments(bool pmReset)
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
        uint32 modType = ItemModType::ITEM_MOD_AGILITY;
        uint32 equipItemClass = 0;
        uint32 equipItemSubClass = 0;        
        if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS)
        {
            equipItemClass = 4;
            if (myLevel < 40)
            {
                equipItemSubClass = 2;
            }
            else
            {
                equipItemSubClass = 3;
            }
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
            equipItemSubClass = 1;
            if (myLevel < 20)
            {
                if (urand(0, 100) > 50)
                {
                    equipItemSubClass = 8;
                }
            }
            else
            {
                uint32 rand = urand(0, 100);
                if (rand > 80)
                {
                    equipItemSubClass = 6;
                }
                else if (rand > 40)
                {
                    equipItemSubClass = 8;
                }
            }
            modType = -1;
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
        {
            equipItemClass = 2;
            equipItemSubClass = 2;
            modType = -1;
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

void NierAction_Hunter::Prepare()
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
    if (!me->HasItemCount(ammoEntry, 1000))
    {
        me->StoreNewItemInBestSlots(ammoEntry, 1000);
    }
    me->SetAmmo(ammoEntry);

    if (Pet* myPet = me->GetPet())
    {
        if (myPet->GetReactState() != ReactStates::REACT_DEFENSIVE)
        {
            myPet->SetReactState(ReactStates::REACT_DEFENSIVE);
        }
        for (PetSpellMap::iterator itr = myPet->m_spells.begin(); itr != myPet->m_spells.end(); itr++)
        {
            if (const SpellInfo* pS = sSpellMgr->GetSpellInfo(itr->first))
            {
                myPet->ToggleAutocast(pS, false);
            }
        }
        myPet->SetPower(Powers::POWER_HAPPINESS, HAPPINESS_LEVEL_SIZE * 2);
    }

    me->Say("Prepared", Language::LANG_UNIVERSAL);
}

bool NierAction_Hunter::DPS(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack)
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
    else if (!me->IsValidAttackTarget(pmTarget))
    {
        if (me->GetTarget() == pmTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    else if (pmTarget->IsImmunedToDamage(SpellSchoolMask::SPELL_SCHOOL_MASK_NORMAL))
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
    if (CastSpell(pmTarget, spell_HuntersMark, true))
    {
        return true;
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return true;
    }
    if (me->IsWithinMeleeRange(pmTarget))
    {
        me->Attack(pmTarget, true);
    }
    else
    {
        me->AttackStop();
    }
    bool shooting = false;
    if (Spell* spell = me->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL))
    {
        if (spell->m_spellInfo->Id == spell_AutoShot)
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
        else
        {
            me->InterruptSpell(CURRENT_AUTOREPEAT_SPELL, true);
        }
    }
    if (!shooting)
    {
        CastSpell(pmTarget, spell_AutoShot);
    }
    PetAttack(pmTarget);
    if (me->GetHealthPct() < 30.0f)
    {
        HealthPotion();
    }
    float manaPCT = me->GetPowerPct(Powers::POWER_MANA);
    if (manaPCT < 30.0f)
    {
        if (!me->HasAura(spell_AspectOfTheViper))
        {
            if (CastSpell(me, spell_AspectOfTheViper))
            {
                return true;
            }
        }
    }
    else if (manaPCT > 70.0f)
    {
        if (me->HasAura(spell_AspectOfTheViper))
        {
            switch (aspectType)
            {
            case HunterAspectType::HunterAspectType_Hawk:
            {
                if (spell_AspectOfTheDragonhawk > 0)
                {
                    if (CastSpell(me, spell_AspectOfTheDragonhawk, true))
                    {
                        return true;
                    }
                }
                else if (spell_AspectOfTheHawk > 0)
                {
                    if (CastSpell(me, spell_AspectOfTheHawk, true))
                    {
                        return true;
                    }
                }
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
    if (pmTarget->isMoving())
    {
        if (CastSpell(pmTarget, spell_ConcussiveShot, true))
        {
            return true;
        }
    }
    if (CastSpell(pmTarget, spell_SerpentSting, true, true))
    {
        return true;
    }
    if (pmRushing || pmTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
    {
        if (CastSpell(me, spell_RapidFire, true))
        {
            return true;
        }
        //if (CastSpell(pmTarget, spell_ChimeraShot))
        //{
        //    return true;
        //}
        if (CastSpell(pmTarget, spell_ArcaneShot))
        {
            return true;
        }
        //if (!pmInstantOnly)
        //{
        //    if (CastSpell(pmTarget, spell_SteadyShot))
        //    {
        //        return true;
        //    }
        //}
    }

    return true;
}

bool NierAction_Hunter::AOE(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing)
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
    else if (pmTarget->IsImmunedToDamage(SpellSchoolMask::SPELL_SCHOOL_MASK_NORMAL))
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
        if (!movement->Chase(pmTarget, pmDistanceMax, pmDistanceMin, pmHolding, false))
        {
            if (me->GetTarget() == pmTarget->GetGUID())
            {
                ClearTarget();
            }
            return false;
        }
    }
    ChooseTarget(pmTarget);
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > NIER_FAR_DISTANCE)
    {
        return true;
    }
    if (me->GetPowerPct(Powers::POWER_MANA) < 30.0f)
    {
        ManaPotion();
    }
    if (!pmInstantOnly)
    {
        if (spell_Volley > 0)
        {
            if (CastSpell(pmTarget, spell_Volley))
            {
                return true;
            }
        }
    }

    return false;
}

bool NierAction_Hunter::Buff(Unit* pmTarget)
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
    if (me->GetGUID() == pmTarget->GetGUID())
    {
        if (CastSpell(me, spell_TrueshotAura, true))
        {
            return true;
        }
        switch (aspectType)
        {
        case HunterAspectType::HunterAspectType_Hawk:
        {
            if (spell_AspectOfTheDragonhawk > 0)
            {
                if (CastSpell(me, spell_AspectOfTheDragonhawk, true))
                {
                    return true;
                }
            }
            else if (spell_AspectOfTheHawk > 0)
            {
                if (CastSpell(me, spell_AspectOfTheHawk, true))
                {
                    return true;
                }
            }
            break;
        }
        default:
        {
            break;
        }
        }
    }

    return false;
}

bool NierAction_Hunter::Petting(bool pmSummon, bool pmReset)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (spell_CallPet > 0 && spell_DismissPet > 0 && spell_RevivePet > 0 && spell_TameBeast > 0)
    {
        if (pmReset)
        {
            if (Pet* myPet = me->GetPet())
            {
                me->RemovePet(myPet, PET_SAVE_AS_DELETED);
                me->SaveToDB(false, false);
                return true;
            }
            return false;
        }
        Pet* myPet = me->GetPet();
        if (pmSummon)
        {
            if (!myPet)
            {
                if (PetStable* ps = me->GetPetStable())
                {
                    if (ps->CurrentPet)
                    {
                        me->CastSpell(me, spell_CallPet);
                        return true;
                    }
                    else if (ps->UnslottedPets.size() > 0)
                    {
                        if (ps->UnslottedPets[0].Health > 0)
                        {
                            me->CastSpell(me, spell_CallPet);
                            return true;
                        }
                        else
                        {
                            me->CastSpell(me, spell_RevivePet);
                            return true;
                        }
                    }
                }
                uint32 beastEntry = urand(0, sNierManager->tamableBeastMap.size() - 1);
                beastEntry = sNierManager->tamableBeastMap[beastEntry];
                if (myPet = me->CreateTamedPetFrom(beastEntry, spell_TameBeast))
                {
                    myPet->GetMap()->AddToMap(myPet->ToCreature());
                    me->SetMinion(myPet, true);
                    myPet->InitTalentForLevel();
                    myPet->SavePetToDB(PET_SAVE_AS_CURRENT);
                    me->PetSpellInitialize();
                    if (myPet->GetReactState() != ReactStates::REACT_DEFENSIVE)
                    {
                        myPet->SetReactState(ReactStates::REACT_DEFENSIVE);
                    }
                }
            }
            else
            {
                if (!myPet->IsAlive())
                {
                    me->CastSpell(me, spell_RevivePet);
                    return true;
                }
            }
        }
        else
        {
            if (myPet)
            {
                me->CastSpell(me, spell_DismissPet);
                return true;
            }
        }
    }

    return false;
}
