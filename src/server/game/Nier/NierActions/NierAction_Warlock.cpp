#include "NierAction_Warlock.h"
#include "World.h"
#include "Pet.h"

NierAction_Warlock::NierAction_Warlock(Player* pmMe) :NierAction_Base(pmMe)
{
    spell_Shoot = 0;
    spell_Corruption = 0;
    spell_Curse_Of_Agony = 0;
    spell_SummonImp = 0;
    spell_Immolate = 0;
    spell_Shadowbolt = 0;
    spell_RainOfFire = 0;
    spell_FireShield = 0;
}

void NierAction_Warlock::InitializeCharacter(uint32 pmTarGetLevel, uint32 pmSpecialtyTabIndex)
{
    if (!me)
    {
        return;
    }
    specialty = pmSpecialtyTabIndex;

    // Destruction warlock only 
    specialty = 2;

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

    // stave 
    me->learnSpell(227, true);
    // wand
    me->learnSpell(5009, true);

    spell_SummonImp = 688;
    spell_Shoot = 5019;
    spell_Immolate = 348;
    spell_Shadowbolt = 686;

    if (myLevel >= 4)
    {
        spell_Corruption = 172;
    }
    if (myLevel >= 6)
    {
        spell_Shadowbolt = 695;
    }
    if (myLevel >= 8)
    {
        spell_Curse_Of_Agony = 980;
    }
    if (myLevel >= 10)
    {
        spell_Immolate = 707;
    }
    if (myLevel >= 12)
    {
        spell_Shadowbolt = 705;
    }
    if (myLevel >= 14)
    {
        spell_Corruption = 6222;
        spell_FireShield = 2947;
    }
    if (myLevel >= 18)
    {
        spell_Curse_Of_Agony = 1014;
    }
    if (myLevel >= 20)
    {
        spell_Immolate = 1094;
        spell_Shadowbolt = 1088;
        spell_RainOfFire = 5740;
    }
    if (myLevel >= 24)
    {
        spell_Corruption = 6223;
        spell_FireShield = 8316;
    }
    if (myLevel >= 28)
    {
        spell_Curse_Of_Agony = 6217;
        spell_Shadowbolt = 1106;
    }
    if (myLevel >= 30)
    {
        spell_Immolate = 2941;
    }
    if (myLevel >= 34)
    {
        spell_Corruption = 7648;
        spell_RainOfFire = 6219;
        spell_FireShield = 8317;
    }
    if (myLevel >= 36)
    {
        spell_Shadowbolt = 7641;
    }
    if (myLevel >= 38)
    {
        spell_Curse_Of_Agony = 11711;
    }
    if (myLevel >= 40)
    {
        spell_Immolate = 11665;
    }
    if (myLevel >= 44)
    {
        spell_Corruption = 11671;
        spell_Shadowbolt = 11659;
        spell_FireShield = 11770;
    }
    if (myLevel >= 46)
    {
        spell_RainOfFire = 11677;
    }
    if (myLevel >= 48)
    {
        spell_Curse_Of_Agony = 11712;
    }
    if (myLevel >= 50)
    {
        spell_Immolate = 11667;
    }
    if (myLevel >= 52)
    {
        spell_Shadowbolt = 11660;
    }
    if (myLevel >= 54)
    {
        spell_Corruption = 11672;
        spell_FireShield = 11771;
    }
    if (myLevel >= 58)
    {
        spell_Curse_Of_Agony = 11713;
        spell_RainOfFire = 11678;
    }
    if (myLevel >= 60)
    {
        spell_Corruption = 25311;
        spell_Shadowbolt = 11661;
        spell_Immolate = 11668;
    }
    if (myLevel >= 61)
    {
        spell_Shadowbolt = 25307;
        spell_Immolate = 25309;
    }
    if (myLevel >= 64)
    {
        spell_FireShield = 27269;
    }
    if (myLevel >= 65)
    {
        spell_Corruption = 27216;
    }
    if (myLevel >= 67)
    {
        spell_Curse_Of_Agony = 27218;
    }
    if (myLevel >= 69)
    {
        spell_Immolate = 27215;
        spell_Shadowbolt = 27209;
        spell_RainOfFire = 27212;
    }
    if (myLevel >= 71)
    {
        spell_Corruption = 47812;
    }
    if (myLevel >= 72)
    {
        spell_RainOfFire = 47819;
    }
    if (myLevel >= 73)
    {
        spell_Curse_Of_Agony = 47863;
    }
    if (myLevel >= 74)
    {
        spell_Shadowbolt = 47808;
    }
    if (myLevel >= 75)
    {
        spell_Immolate = 47810;
    }
    if (myLevel >= 76)
    {
        spell_FireShield = 47983;
    }
    if (myLevel >= 77)
    {
        spell_Corruption = 47813;
    }
    if (myLevel >= 79)
    {
        spell_Curse_Of_Agony = 47864;
        spell_Shadowbolt = 47809;
        spell_RainOfFire = 47820;
    }
    if (myLevel >= 80)
    {
        spell_Immolate = 47811;
    }
    me->UpdateSkillsToMaxSkillsForLevel();

    std::ostringstream msgStream;
    msgStream << me->GetName() << " initialized";
    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
}

void NierAction_Warlock::ResetTalent()
{
    if (!me)
    {
        return;
    }
    me->resetTalents(true);
    me->SendTalentsInfoData(false);

    switch (specialty)
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
        // destruction 
        LearnTalent(944);
        LearnTalent(943);
        LearnTalent(982);
        LearnTalent(941);
        LearnTalent(983);
        LearnTalent(963);
        LearnTalent(967);
        LearnTalent(964);
        LearnTalent(961);
        LearnTalent(981);
        LearnTalent(966);
        LearnTalent(968);
        LearnTalent(986);
        LearnTalent(1677);
        LearnTalent(1888);
        LearnTalent(1676);
        LearnTalent(2045);
        LearnTalent(1890);
        LearnTalent(1891);
        break;
    }
    default:
    {
        break;
    }
    }

    me->SendTalentsInfoData(false);

    // warlock trainer
    TrainSpells(5173);
}

bool NierAction_Warlock::InitializeEquipments(bool pmReset)
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

void NierAction_Warlock::Prepare()
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
                if (itr->first == 4511)
                {
                    myPet->ToggleAutocast(pS, false);
                }
                else
                {
                    myPet->ToggleAutocast(pS, true);
                }
            }
        }
    }

    me->Say("Prepared", Language::LANG_UNIVERSAL);
}

bool NierAction_Warlock::DPS(Unit* pTarget, bool pRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack)
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
    if (!pTarget)
    {
        return false;
    }
    else if (!pTarget->IsAlive())
    {
        if (me->GetTarget() == pTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    else if (!me->IsValidAttackTarget(pTarget))
    {
        if (me->GetTarget() == pTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    else if (pTarget->IsImmunedToDamage(SpellSchoolMask::SPELL_SCHOOL_MASK_NORMAL))
    {
        if (me->GetTarget() == pTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    if (!pTarget->CanSeeOrDetect(me))
    {
        if (me->GetTarget() == pTarget->GetGUID())
        {
            ClearTarget();
        }
        return false;
    }
    if (pmChasing)
    {
        if (!movement->Chase(pTarget, pmDistanceMax, pmDistanceMin, pmHolding, pmForceBack))
        {
            if (me->GetTarget() == pTarget->GetGUID())
            {
                ClearTarget();
            }
            return false;
        }
    }
    ChooseTarget(pTarget);
    float targetDistance = me->GetDistance(pTarget);
    if (targetDistance > NIER_MAX_DISTANCE)
    {
        return true;
    }

    if (spell_Immolate > 0)
    {
        if (CastSpell(pTarget, spell_Immolate, true, true))
        {
            return true;
        }
    }
    if (pRushing || pTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
    {
        if (spell_Shadowbolt > 0)
        {
            if (CastSpell(pTarget, spell_Shadowbolt))
            {
                return true;
            }
        }
    }
    if (spell_Shoot > 0)
    {
        if (Spell* shooting = me->GetCurrentSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL))
        {
            return true;
        }
        else
        {
            if (CastSpell(pTarget, spell_Shoot))
            {
                return true;
            }
        }
    }

    return true;
}

bool NierAction_Warlock::AOE(Unit* pTarget, bool pRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing)
{
    return false;
}

bool NierAction_Warlock::Buff(Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (!pTarget)
    {
        return false;
    }
    if (me->GetGUID() == pTarget->GetGUID())
    {
        if (Pet* myPet = me->GetPet())
        {

        }
        else
        {
            if (CastSpell(me, spell_SummonImp))
            {
                return true;
            }
        }
    }

    return false;
}

bool NierAction_Warlock::Petting(bool pmSummon, bool pmReset)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmSummon)
    {
        Pet* myPet = me->GetPet();
        if (!myPet)
        {
            if (CastSpell(me, spell_SummonImp))
            {
                return true;
            }
        }
    }

    return false;
}
