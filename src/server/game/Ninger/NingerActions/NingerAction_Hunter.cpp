#include "NingerAction_Hunter.h"
#include "Pet.h"
#include "Group.h"
#include "Spell.h"

NingerAction_Hunter::NingerAction_Hunter() :NingerAction_Base()
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

void NingerAction_Hunter::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
{
    if (!me)
    {
        return;
    }
    specialty = pmSpecialtyTabIndex;
    me->ClearInCombat();
    uint32 myLevel = me->getLevel();
    if (myLevel != pmTargetLevel)
    {
        me->GiveLevel(pmTargetLevel);
        me->LearnDefaultSkills();
        me->LearnCustomSpells();

        ResetTalent();
    }
    spell_AutoShot = 75;
    me->learnSpell(197);
    me->learnSpell(264);
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
        me->CastSpell(me, 23356);
        me->CastSpell(me, 23357);
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
        me->learnSpell(200);
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
    InitializeEquipments(myLevel != pmTargetLevel);
    std::ostringstream msgStream;
    msgStream << me->GetName() << " initialized";
    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
}

void NingerAction_Hunter::ResetTalent()
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

void NingerAction_Hunter::InitializeEquipments(bool pmReset)
{
    if (!me)
    {
        return;
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
            if (myLevel < 20)
            {
                equipItemSubClass = 1;
            }
            else
            {
                equipItemSubClass = 6;
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_RANGED)
        {
            equipItemClass = 2;
            equipItemSubClass = 2;
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
        EquipRandomItem(checkEquipSlot, equipItemClass, equipItemSubClass, minQuality, me->getLevel(), modType);
    }
}

void NingerAction_Hunter::Prepare()
{
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
                myPet->ToggleAutocast(pS, true);
            }
        }
    }
    me->Say("Prepared", Language::LANG_UNIVERSAL);
}

bool NingerAction_Hunter::DPS(Unit* pmTarget, bool pmAOE, float pmChaseDistanceMin, float pmChaseDistanceMax)
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
        if (!me->IsValidAttackTarget(pmTarget))
        {
            me->InterruptSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
            return false;
        }
    }
    float targetDistance = me->GetDistance(pmTarget);
    if (targetDistance > VISIBILITY_DISTANCE_NORMAL)
    {
        return false;
    }
    rm->Chase(pmTarget, pmChaseDistanceMin, pmChaseDistanceMax);
    if (CastSpell(pmTarget, spell_HuntersMark, true))
    {
        return true;
    }
    if (!rm->chasePositionOK)
    {
        return true;
    }
    bool shooting = false;
    if (Spell* spell = me->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL))
    {
        if (spell->m_spellInfo->Id == spell_AutoShot)
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
        CastSpell(pmTarget, spell_AutoShot);
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
    if (CastSpell(pmTarget, spell_ChimeraShot))
    {
        return true;
    }
    if (CastSpell(pmTarget, spell_ArcaneShot))
    {
        return true;
    }
    if (CastSpell(pmTarget, spell_SteadyShot))
    {
        return true;
    }

    return false;
}

bool NingerAction_Hunter::Buff(Unit* pmTarget)
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

bool NingerAction_Hunter::Petting(bool pmSummon, bool pmReset)
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
                    if (ps->CurrentPet || ps->UnslottedPets.size() > 0)
                    {
                        me->CastSpell(me, spell_CallPet);
                        return true;
                    }
                }
                uint32 beastEntry = 0;
                QueryResult beastQR = WorldDatabase.Query("SELECT entry FROM creature_template where type = 1 and type_flags & 1 order by rand()");
                if (beastQR)
                {
                    do
                    {
                        Field* fields = beastQR->Fetch();
                        beastEntry = fields[0].Get<uint32>();
                        if (myPet = me->CreatePet(beastEntry, spell_TameBeast))
                        {
                            myPet->GetMap()->AddToMap(myPet->ToCreature(), true);
                            me->SetMinion(myPet, true);
                            myPet->InitTalentForLevel();
                            myPet->SavePetToDB(PET_SAVE_AS_CURRENT);
                            me->PetSpellInitialize();
                            break;
                        }
                    } while (beastQR->NextRow());
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

void NingerAction_Hunter::PetStop()
{
    if (Pet* myPet = me->GetPet())
    {
        myPet->AttackStop();
        if (CharmInfo* ci = myPet->GetCharmInfo())
        {
            ci->SetIsCommandAttack(false);
            ci->SetIsCommandFollow(true);
        }
    }
}
