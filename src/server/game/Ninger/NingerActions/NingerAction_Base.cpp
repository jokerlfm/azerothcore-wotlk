#include "NingerAction_Base.h"
#include "Bag.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "CreatureAI.h"
#include "MotionMaster.h"
#include "SpellMgr.h"
#include "Pet.h"

NingerMovement::NingerMovement(Player* pmMe)
{
    me = pmMe;
}

void NingerMovement::ResetMovement()
{

}

void NingerMovement::Update(uint32 pmDiff)
{

}

bool NingerMovement::Chase(Unit* pmChaseTarget, float pmChaseDistanceMin, float pmChaseDistanceMax, uint32 pmLimitDelay)
{
    return false;
}

void NingerMovement::MovePoint(Position pmTargetPosition, uint32 pmLimitDelay)
{

}

void NingerMovement::MovePoint(float pmX, float pmY, float pmZ, uint32 pmLimitDelay)
{

}

void NingerMovement::MoveTargetPosition()
{

}

void NingerMovement::MoveTargetPosition(float pmX, float pmY, float pmZ)
{

}


NingerAction_Base::NingerAction_Base(Player* pmMe)
{
    me = pmMe;
    rm = new NingerMovement(me);
    maxTalentTab = 0;
    chaseDistanceMin = 1.0f;
    chaseDistanceMax = 3.0f;
    rti = 0;
}

void NingerAction_Base::Reset()
{

}

void NingerAction_Base::Update(uint32 pmDiff)
{

}

bool NingerAction_Base::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
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

void NingerAction_Base::Prepare()
{

}

void LearnTalents(uint32 pmTabIndex)
{

}

void NingerAction_Base::InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex)
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
        LearnTalents(pmSpecialtyTabIndex);
        // todo : learn spells

        // todo : initial equipments
        InitializeEquipments(true);
    }    
    std::ostringstream msgStream;
    msgStream << me->GetName() << " initialized";
    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
}

void NingerAction_Base::InitializeEquipments(bool pmReset)
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
        if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
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
                if (me->getLevel() < 20)
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
            std::unordered_set<uint32> usableItemClass;
            std::unordered_set<uint32> usableItemSubClass;
            usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
            usableItemSubClass.insert(GetUsableArmorSubClass(me));
            TryEquip(me, usableItemClass, usableItemSubClass, checkEquipSlot);
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_MAINHAND)
        {
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
            int weaponSubClass_mh = -1;
            int weaponSubClass_oh = -1;
            int weaponSubClass_r = -1;
            switch (me->getClass())
            {
            case Classes::CLASS_WARRIOR:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
                weaponSubClass_oh = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_SHIELD;
                break;
            }
            case Classes::CLASS_PALADIN:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_MACE;
                uint32 weaponType = urand(0, 100);
                if (weaponType < 50)
                {
                    weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
                }
                weaponSubClass_oh = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_SHIELD;
                break;
            }
            case Classes::CLASS_HUNTER:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_AXE2;
                uint32 rType = urand(0, 2);
                if (rType == 0)
                {
                    weaponSubClass_r = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_BOW;
                }
                else if (rType == 1)
                {
                    weaponSubClass_r = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_CROSSBOW;
                }
                else
                {
                    weaponSubClass_r = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_GUN;
                }
                break;
            }
            case Classes::CLASS_ROGUE:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
                weaponSubClass_oh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
                break;
            }
            case Classes::CLASS_PRIEST:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
                break;
            }
            case Classes::CLASS_SHAMAN:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_MACE;
                weaponSubClass_oh = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_SHIELD;
                break;
            }
            case Classes::CLASS_MAGE:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
                break;
            }
            case Classes::CLASS_WARLOCK:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
                break;
            }
            case Classes::CLASS_DRUID:
            {
                weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
                break;
            }
            default:
            {
                continue;
            }
            }
            if (weaponSubClass_mh >= 0)
            {
                std::unordered_set<uint32> usableItemClass;
                std::unordered_set<uint32> usableItemSubClass;
                usableItemClass.insert(ItemClass::ITEM_CLASS_WEAPON);
                usableItemSubClass.insert(weaponSubClass_mh);
                TryEquip(me, usableItemClass, usableItemSubClass, checkEquipSlot);
            }
            if (weaponSubClass_oh >= 0)
            {
                std::unordered_set<uint32> usableItemClass;
                std::unordered_set<uint32> usableItemSubClass;
                usableItemClass.insert(ItemClass::ITEM_CLASS_WEAPON);
                usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
                usableItemSubClass.insert(weaponSubClass_oh);
                TryEquip(me, usableItemClass, usableItemSubClass, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND);
            }
            if (weaponSubClass_r >= 0)
            {
                std::unordered_set<uint32> usableItemClass;
                std::unordered_set<uint32> usableItemSubClass;
                usableItemClass.insert(ItemClass::ITEM_CLASS_WEAPON);
                usableItemSubClass.insert(weaponSubClass_r);
                TryEquip(me, usableItemClass, usableItemSubClass, EquipmentSlots::EQUIPMENT_SLOT_RANGED);
            }
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_BACK)
        {
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
            std::unordered_set<uint32> usableItemClass;
            std::unordered_set<uint32> usableItemSubClass;
            usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
            usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH);
            TryEquip(me, usableItemClass, usableItemSubClass, checkEquipSlot);
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1)
        {
            if (me->getLevel() < 20)
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
            std::unordered_set<uint32> usableItemClass;
            std::unordered_set<uint32> usableItemSubClass;
            usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
            usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MISC);
            TryEquip(me, usableItemClass, usableItemSubClass, checkEquipSlot);
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
        {
            if (me->getLevel() < 20)
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
            std::unordered_set<uint32> usableItemClass;
            std::unordered_set<uint32> usableItemSubClass;
            usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
            usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MISC);
            TryEquip(me, usableItemClass, usableItemSubClass, checkEquipSlot);
        }
        else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
        {
            if (me->getLevel() < 30)
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
            std::unordered_set<uint32> usableItemClass;
            std::unordered_set<uint32> usableItemSubClass;
            usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
            usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MISC);
            TryEquip(me, usableItemClass, usableItemSubClass, checkEquipSlot);
        }
    }
}

/*
NingerMovement::NingerMovement(Player* pmMe)
{
    me = pmMe;
    ogChaseTarget = ObjectGuid::Empty;
    activeMovementType = NingerMovementType::NingerMovementType_None;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
}

void NingerMovement::ResetMovement()
{
    ogChaseTarget = ObjectGuid::Empty;
    activeMovementType = NingerMovementType::NingerMovementType_None;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;
    if (me)
    {
        me->StopMoving();
        me->GetMotionMaster()->Initialize();
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
        ResetMovement();
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

Script_Base::Script_Base(Player* pmMe)
{
    me = pmMe;
    rm = new NingerMovement(me);
    maxTalentTab = 0;
    buffDelay = 0;
    healDelay = 0;
    cureDelay = 0;
    potionDelay = 0;
    aoeCheckDelay = 0;
    chaseDistanceMin = MELEE_MIN_DISTANCE;
    chaseDistanceMax = MELEE_MAX_DISTANCE;

    rti = -1;
    ogReviveTarget.Clear();
}

void Script_Base::Initialize()
{

}

void Script_Base::Reset()
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
    ClearTarget();
    buffDelay = 0;
    healDelay = 0;
    cureDelay = 0;
    potionDelay = 0;
    aoeCheckDelay = 0;
}

bool Script_Base::Revive(Player* pmTarget)
{
    return false;
}

void Script_Base::Update(uint32 pmDiff)
{
    if (buffDelay >= 0)
    {
        buffDelay -= pmDiff;
    }
    if (healDelay >= 0)
    {
        healDelay -= pmDiff;
    }
    if (cureDelay >= 0)
    {
        cureDelay -= pmDiff;
    }
    if (potionDelay >= 0)
    {
        potionDelay -= pmDiff;
    }
    if (aoeCheckDelay >= 0)
    {
        aoeCheckDelay -= pmDiff;
    }
    rm->Update(pmDiff);
    return;
}

bool Script_Base::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    return false;
}

bool Script_Base::Tank(Unit* pmTarget, bool pmChase, bool pmAOE)
{
    return false;
}

bool Script_Base::Heal(Unit* pmTarget)
{
    return false;
}

bool Script_Base::Cure(Unit* pmTarget)
{
    return false;
}

bool Script_Base::Buff(Unit* pmTarget)
{
    return false;
}

bool Script_Base::Petting(bool pmSummon)
{
    return false;
}

bool Script_Base::Assist(Unit* pmTarget)
{
    return false;
}

Item* Script_Base::GetItemInInventory(uint32 pmEntry)
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

bool Script_Base::UseItem(Item* pmItem, Unit* pmTarget)
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

bool Script_Base::UseItem(Item* pmItem, Item* pmTarget)
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
        SpellCastTargets targets;
        targets.SetItemTarget(pmTarget);
        me->CastItemUseSpell(pmItem, targets, 1, 0);
        return true;
    }

    return false;
}

bool Script_Base::Follow(Unit* pmTarget, float pmDistance)
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
    return rm->Chase(pmTarget, 0.0f, pmDistance);
}

bool Script_Base::Chase(Unit* pmTarget, float pmMinDistance, float pmMaxDistance)
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

bool Script_Base::SpellValid(uint32 pmSpellID)
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

uint32 Script_Base::GetMaxRankSpellId(uint32 pmBaseSpellID, uint32 pmPlayerLevel)
{
    return 0;
}

bool Script_Base::CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura, bool pmOnlyMyAura, bool pmClearShapeShift)
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
    uint32 spellID = GetMaxRankSpellId(pmSpellId, me->getLevel());
    if (!SpellValid(spellID))
    {
        return false;
    }
    const SpellInfo* pS = sSpellMgr->GetSpellInfo(spellID);
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
                if (sNingerManager->HasAura(pmTarget, pmSpellId, me))
                {
                    return false;
                }
            }
            else
            {
                if (sNingerManager->HasAura(pmTarget, pmSpellId))
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
    SpellCastResult scr = me->CastSpell(pmTarget, pS);
    if (scr == SpellCastResult::SPELL_CAST_OK)
    {
        return true;
    }

    return false;
}

void Script_Base::ClearShapeshift()
{
    if (!me)
    {
        return;
    }
    uint32 spellID = 0;
    switch (me->GetShapeshiftForm())
    {
    case ShapeshiftForm::FORM_NONE:
    {
        break;
    }
    case ShapeshiftForm::FORM_CAT:
    {
        spellID = GetMaxRankSpellId(BaseSpell::CatForm, me->getLevel());
        break;
    }
    case ShapeshiftForm::FORM_DIREBEAR:
    {
        spellID = GetMaxRankSpellId(BaseSpell::DireBearForm, me->getLevel());
        break;
    }
    case ShapeshiftForm::FORM_BEAR:
    {
        spellID = GetMaxRankSpellId(BaseSpell::BearForm, me->getLevel());
        break;
    }
    case ShapeshiftForm::FORM_MOONKIN:
    {
        spellID = GetMaxRankSpellId(BaseSpell::MoonkinForm, me->getLevel());
        break;
    }
    default:
    {
        break;
    }
    }
    CancelAura(spellID);
}

void Script_Base::CancelAura(uint32 pmSpellID)
{
    if (pmSpellID == 0)
    {
        return;
    }
    if (!me)
    {
        return;
    }
    uint32 activeSpellId = sNingerManager->HasAura(me, pmSpellID);
    if (activeSpellId > 0)
    {
        me->RemoveAurasDueToSpell(activeSpellId);
    }
}

bool Script_Base::Eat(bool pmForce)
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
    bool result = pmForce;
    if (!result)
    {
        if (me->GetHealthPct() < 40.0f)
        {
            result = true;
        }
    }
    if (result)
    {
        uint32 foodEntry = 0;
        uint32 myLevel = me->getLevel();
        if (myLevel >= 85)
        {
            foodEntry = 58261;
        }
        else if (myLevel >= 80)
        {
            foodEntry = 58260;
        }
        else if (myLevel >= 75)
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
        if (result)
        {
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
                }
            }
        }
    }
    else
    {
        if (me->getPowerType() == Powers::POWER_MANA)
        {
            if (me->GetPower(Powers::POWER_MANA) * 100 / me->GetMaxPower(Powers::POWER_MANA) < 40.0f)
            {
                result = true;
            }
        }
    }
    return result;
}

bool Script_Base::Drink()
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
    if (myLevel >= 85)
    {
        drinkEntry = 58257;
    }
    else if (myLevel >= 80)
    {
        drinkEntry = 58256;
    }
    else if (myLevel >= 75)
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

void Script_Base::PetAttack(Unit* pmTarget)
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

void Script_Base::PetStop()
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

bool Script_Base::UseHealingPotion()
{
    bool result = false;

    if (potionDelay > 61000)
    {
        if (!me)
        {
            return false;
        }
        if (!me->IsInCombat())
        {
            return false;
        }
        uint32 itemEntry = 0;
        uint32 myLevel = me->getLevel();
        if (myLevel >= 45)
        {
            itemEntry = 13446;
        }
        else if (myLevel >= 35)
        {
            itemEntry = 3928;
        }
        else if (myLevel >= 21)
        {
            itemEntry = 1710;
        }
        else if (myLevel >= 12)
        {
            itemEntry = 929;
        }
        else
        {
            itemEntry = 118;
        }
        if (!me->HasItemCount(itemEntry, 1))
        {
            me->StoreNewItemInBestSlots(itemEntry, 20);
        }
        Item* pItem = GetItemInInventory(itemEntry);
        if (pItem && !pItem->IsInTrade())
        {
            if (UseItem(pItem, me))
            {
                potionDelay = 0;
                result = true;
            }
        }

    }

    return result;
}

bool Script_Base::UseManaPotion()
{
    bool result = false;

    if (potionDelay < 0)
    {
        potionDelay = 2000;
        if (!me)
        {
            return false;
        }
        if (!me->IsInCombat())
        {
            return false;
        }
        uint32 itemEntry = 0;
        uint32 myLevel = me->getLevel();
        if (myLevel >= 49)
        {
            itemEntry = 13444;
        }
        else if (myLevel >= 41)
        {
            itemEntry = 13443;
        }
        else if (myLevel >= 31)
        {
            itemEntry = 6149;
        }
        else if (myLevel >= 22)
        {
            itemEntry = 3827;
        }
        else if (myLevel >= 14)
        {
            itemEntry = 3385;
        }
        else
        {
            itemEntry = 2455;
        }
        if (!me->HasItemCount(itemEntry, 1))
        {
            me->StoreNewItemInBestSlots(itemEntry, 20);
        }
        Item* pItem = GetItemInInventory(itemEntry);
        if (pItem && !pItem->IsInTrade())
        {
            if (UseItem(pItem, me))
            {
                potionDelay = 120000;
                result = true;
            }
        }

    }

    return result;
}

void Script_Base::ChooseTarget(Unit* pmTarget)
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

void Script_Base::ClearTarget()
{
    if (me)
    {
        me->SetSelection(ObjectGuid::Empty);
        me->SetTarget(ObjectGuid::Empty);
    }
}

void Script_Base::InitializeTalents()
{
    if (!me)
    {
        return;
    }
    me->resetTalents(true);
    bool newPlayer = false;
    uint32 freePoint = me->GetFreeTalentPoints();
    if (freePoint > 0)
    {
        if (freePoint == me->getLevel() - 9)
        {
            newPlayer = true;
        }
        uint8 specialty = urand(0, 2);        
        uint32 targetClass = me->getClass();
        if (targetClass == Classes::CLASS_MAGE)
        {
            specialty = 2;
        }
        else if (targetClass == Classes::CLASS_ROGUE)
        {
            specialty = 1;
        }
        else if (targetClass == Classes::CLASS_WARRIOR)
        {
            specialty = 2;
        }
        else if (targetClass == Classes::CLASS_SHAMAN)
        {
            specialty = 2;
        }
        else if (targetClass == Classes::CLASS_PRIEST)
        {
            specialty = 0;
        }
        else if (targetClass == Classes::CLASS_WARLOCK)
        {
            specialty = 2;
        }
        else if (targetClass == Classes::CLASS_PALADIN)
        {
            specialty = 0;
        }
        else if (targetClass == Classes::CLASS_DRUID)
        {
            specialty = 1;
        }
        else if (targetClass == Classes::CLASS_HUNTER)
        {
            specialty = 1;
        }
        // todo const talent values 
        if (targetClass == Classes::CLASS_MAGE)
        {
            if (specialty == 0)
            {
                //me->LearnTalent(eachTE->ID, maxRank);
            }
            else if (specialty == 1)
            {
                //me->LearnTalent(eachTE->ID, maxRank);
            }
            else if (specialty == 2)
            {
                //me->LearnTalent(eachTE->ID, maxRank);
            }
        }
        else if (targetClass == Classes::CLASS_ROGUE)
        {

        }
        else if (targetClass == Classes::CLASS_WARRIOR)
        {

        }
        else if (targetClass == Classes::CLASS_SHAMAN)
        {

        }
        else if (targetClass == Classes::CLASS_PRIEST)
        {

        }
        else if (targetClass == Classes::CLASS_WARLOCK)
        {

        }
        else if (targetClass == Classes::CLASS_PALADIN)
        {

        }
        else if (targetClass == Classes::CLASS_DRUID)
        {

        }
        else if (targetClass == Classes::CLASS_HUNTER)
        {

        }
        me->SaveToDB(false, false);
    }
}

void Script_Base::LearnPlayerSpells(Player* me)
{
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        CreatureTemplate cInfo = itr->second;
        if (cInfo.trainer_class == me->getClass())
        {
            if (cInfo.trainer_type == TrainerType::TRAINER_TYPE_CLASS)
            {
                TrainerSpellData const* trainer_spells = sObjectMgr->GetNpcTrainerSpells(cInfo.Entry);
                if (trainer_spells)
                {
                    while (true)
                    {
                        bool hasNew = false;
                        for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
                        {
                            TrainerSpell const* tSpell = &itr->second;

                            bool valid = true;
                            bool primary_prof_first_rank = false;
                            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                            {
                                if (!tSpell->learnedSpell[i])
                                    continue;
                                if (!me->IsSpellFitByClassAndRace(tSpell->learnedSpell[i]))
                                {
                                    valid = false;
                                    break;
                                }
                                SpellInfo const* learnedSpellInfo = sSpellMgr->GetSpellInfo(tSpell->learnedSpell[i]);
                                if (learnedSpellInfo && learnedSpellInfo->IsPrimaryProfessionFirstRank())
                                    primary_prof_first_rank = true;
                            }

                            if (!valid)
                                continue;

                            if (tSpell->reqSpell && !me->HasSpell(tSpell->reqSpell))
                            {
                                continue;
                            }

                            TrainerSpellState state = me->GetTrainerSpellState(tSpell);
                            if (state == TRAINER_SPELL_GREEN)
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
                }
            }
        }
    }
}

uint32 Script_Base::GetUsableArmorSubClass(Player* me)
{
    if (!me)
    {
        return false;
    }
    uint32 resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
    switch (me->getClass())
    {
    case Classes::CLASS_WARRIOR:
    {
        if (me->getLevel() < 40)
        {
            // use mail armor
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
        }
        else
        {
            // use plate armor
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_PLATE;
        }
        break;
    }
    case Classes::CLASS_PALADIN:
    {
        if (me->getLevel() < 40)
        {
            // use mail armor
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
        }
        else
        {
            // use plate armor
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_PLATE;
        }
        break;
    }
    case Classes::CLASS_HUNTER:
    {
        if (me->getLevel() < 40)
        {
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
        }
        else
        {
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
        }
        break;
    }
    case Classes::CLASS_ROGUE:
    {
        resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
        break;
    }
    case Classes::CLASS_PRIEST:
    {
        resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
        break;
    }
    case Classes::CLASS_SHAMAN:
    {
        if (me->getLevel() < 40)
        {
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
        }
        else
        {
            resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
        }
        break;
    }
    case Classes::CLASS_MAGE:
    {
        resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
        break;
    }
    case Classes::CLASS_WARLOCK:
    {
        resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
        break;
    }
    case Classes::CLASS_DRUID:
    {
        resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
        break;
    }
    default:
    {
        break;
    }
    }

    return resultArmorSubClass;
}

bool Script_Base::EquipNewItem(Player* me, uint32 pmItemEntry, uint8 pmEquipSlot)
{
    if (!me)
    {
        return false;
    }
    uint16 eDest;
    InventoryResult tryEquipResult = me->CanEquipNewItem(NULL_SLOT, eDest, pmItemEntry, false);
    if (tryEquipResult == EQUIP_ERR_OK)
    {
        ItemPosCountVec sDest;
        InventoryResult storeResult = me->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, pmItemEntry, 1);
        if (storeResult == EQUIP_ERR_OK)
        {
            Item* pItem = me->StoreNewItem(sDest, pmItemEntry, true, Item::GenerateItemRandomPropertyId(pmItemEntry));
            if (pItem)
            {
                InventoryResult equipResult = me->CanEquipItem(NULL_SLOT, eDest, pItem, false);
                if (equipResult == EQUIP_ERR_OK)
                {
                    me->RemoveItem(INVENTORY_SLOT_BAG_0, pItem->GetSlot(), true);
                    me->EquipItem(pmEquipSlot, pItem, true);
                    return true;
                }
                else
                {
                    pItem->DestroyForPlayer(me);
                }
            }
        }
    }

    return false;
}

void Script_Base::TryEquip(Player* me, std::unordered_set<uint32> pmClassSet, std::unordered_set<uint32> pmSubClassSet, uint32 pmTargetSlot)
{
    if (!me)
    {
        return;
    }
    uint32 minQuality = ItemQualities::ITEM_QUALITY_UNCOMMON;
    if (me->getLevel() < 20)
    {
        minQuality = ItemQualities::ITEM_QUALITY_POOR;
    }
    std::unordered_map<uint32, uint32> validEquipSet;
    for (std::unordered_set<uint32>::iterator ieIT = lootItemEntrySet.begin(); ieIT != lootItemEntrySet.end(); ieIT++)
    {
        uint32 eachItemEntry = *ieIT;
        const ItemTemplate* proto = sObjectMgr->GetItemTemplate(eachItemEntry);
        if (!proto)
        {
            continue;
        }
        if (pmClassSet.find(proto->Class) == pmClassSet.end())
        {
            continue;
        }
        if (pmSubClassSet.find(proto->SubClass) == pmSubClassSet.end())
        {
            continue;
        }
        if (proto->Quality < minQuality || proto->Quality > ItemQualities::ITEM_QUALITY_EPIC)
        {
            continue;
        }
        // test items
        if (proto->ItemId == 19879)
        {
            continue;
        }
        if (proto->Class == ItemClass::ITEM_CLASS_WEAPON)
        {
            if (proto->Damage[0].DamageMin > 10.0f && proto->Damage[0].DamageMax > 10.0f)
            {
                // valid weapon 
            }
            else
            {
                continue;
            }
            if (proto->SubClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF)
            {
                if (me->getClass() == Classes::CLASS_WARLOCK || me->getClass() == Classes::CLASS_PRIEST || me->getClass() == Classes::CLASS_MAGE)
                {
                    bool hasSpellPower = false;
                    for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
                    {
                        if (proto->ItemStat[i].ItemStatType == ItemModType::ITEM_MOD_SPELL_POWER)
                        {
                            hasSpellPower = true;
                            break;
                        }
                    }
                    if (!hasSpellPower)
                    {
                        continue;
                    }
                }
            }
            else if (proto->SubClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD)
            {
                bool hasSpellPower = false;
                for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
                {
                    if (proto->ItemStat[i].ItemStatType == ItemModType::ITEM_MOD_SPELL_POWER)
                    {
                        hasSpellPower = true;
                        break;
                    }
                }
                if (hasSpellPower)
                {
                    continue;
                }
            }
        }
        std::unordered_set<uint32> usableSlotSet = GetUsableEquipSlot(proto);
        if (usableSlotSet.find(pmTargetSlot) != usableSlotSet.end())
        {
            uint32 checkMinRequiredLevel = me->getLevel();
            if (checkMinRequiredLevel > 10)
            {
                checkMinRequiredLevel = checkMinRequiredLevel - 5;
            }
            else
            {
                checkMinRequiredLevel = 5;
            }
            if (proto->RequiredLevel <= me->getLevel() && proto->RequiredLevel >= checkMinRequiredLevel)
            {
                validEquipSet[validEquipSet.size()] = proto->ItemId;
            }
        }
    }
    if (validEquipSet.size() > 0)
    {
        int tryTimes = 5;
        while (tryTimes > 0)
        {
            uint32 equipEntry = urand(0, validEquipSet.size() - 1);
            equipEntry = validEquipSet[equipEntry];
            if (EquipNewItem(me, equipEntry, pmTargetSlot))
            {
                break;
            }
            tryTimes--;
        }
    }
}

bool Script_Base::RandomTeleport(Player* me)
{
    if (!me)
    {
        return false;
    }
    if (me->IsBeingTeleported())
    {
        return false;
    }
    if (!me->IsAlive())
    {
        me->ResurrectPlayer(1.0f);
        me->SpawnCorpseBones();
    }
    me->ClearInCombat();
    me->StopMoving();
    uint32 targetLevel = me->getLevel();
    std::unordered_map<uint32, uint32> onlinePlayerZoneIDMap;
    std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
    for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
    {
        if (WorldSession* eachWS = wsIT->second)
        {
            if (!eachWS->isNinger)
            {
                if (Player* eachPlayer = eachWS->GetPlayer())
                {
                    uint32 playerZoneID = eachPlayer->GetZoneId();
                    if (pvpPositionMap.find(playerZoneID) != pvpPositionMap.end())
                    {
                        if (targetLevel >= pvpPositionMap[playerZoneID]->minLevel && targetLevel <= pvpPositionMap[playerZoneID]->maxLevel)
                        {
                            onlinePlayerZoneIDMap[onlinePlayerZoneIDMap.size()] = playerZoneID;
                        }
                    }
                }
            }
        }
    }
    if (onlinePlayerZoneIDMap.size() > 0)
    {
        uint32 destZoneID = urand(0, onlinePlayerZoneIDMap.size() - 1);
        destZoneID = onlinePlayerZoneIDMap[destZoneID];
        if (pvpZonePosition* destZone = pvpPositionMap[destZoneID])
        {
            float destX = 0.0f, destY = 0.0f, destZ = 0.0f;
            uint32 positionIndex = urand(0, destZone->flagPointMap.size() - 1);
            Position destPosition = destZone->flagPointMap[positionIndex];
            destX = frand(destPosition.m_positionX - destZone->spawnDistance, destPosition.m_positionX + destZone->spawnDistance);
            destY = frand(destPosition.m_positionY - destZone->spawnDistance, destPosition.m_positionY + destZone->spawnDistance);
            destZ = destPosition.m_positionZ;
            TeleportPlayer(me, destZone->mapID, destX, destY, destZ);
            sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "PVP Teleport ninger %s (level %d)", me->GetName(), me->getLevel());
            return true;
        }
    }
    me->TeleportTo(me->m_homebindMapId, me->m_homebindX, me->m_homebindY, me->m_homebindZ, 0.0f);
    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Home Teleport ninger %s (level %d)", me->GetName(), me->getLevel());
    return true;
}

void Script_Base::TeleportTo(Player* me, uint32 pmMapID, float pmDestX, float pmDestY, float pmDestZ)
{
    if (!me)
    {
        return;
    }
    if (me->IsBeingTeleported())
    {
        return;
    }
    me->ClearInCombat();
    me->StopMoving();
    me->GetMotionMaster()->Initialize();
    me->TeleportTo(me->m_homebindMapId, me->m_homebindX, me->m_homebindY, me->m_homebindZ, 0.0f);
    if (Awareness_Base* ab = me->awarenessMap[me->activeAwarenessIndex])
    {
        ab->teleportDelay = 5000;
        ab->wlTeleportDestination.m_mapId = pmMapID;
        ab->wlTeleportDestination.m_positionX = pmDestX;
        ab->wlTeleportDestination.m_positionY = pmDestY;
        ab->wlTeleportDestination.m_positionZ = pmDestZ;
    }
}

uint32 Script_Base::HasAura(Unit* pmTarget, uint32 pmSpellId, Unit* pmCaster)
{
    if (!pmTarget)
    {
        return 0;
    }
    if (const SpellInfo* pS = sSpellMgr->GetSpellInfo(pmSpellId))
    {
        ObjectGuid ogCaster = ObjectGuid::Empty;
        if (pmCaster)
        {
            ogCaster = pmCaster->GetGUID();
        }
        if (pS->IsRanked())
        {
            const SpellInfo* pSIEachRank = pS->GetFirstRankSpell();
            while (pSIEachRank)
            {
                if (pmTarget->HasAura(pSIEachRank->Id, ogCaster))
                {
                    return pSIEachRank->Id;
                }
                pSIEachRank = pSIEachRank->GetNextRankSpell();
            }
        }
        else if (pmTarget->HasAura(pS->Id, ogCaster))
        {
            return pS->Id;
        }
    }
    return 0;
}

uint32 Script_Base::GetAuraDuration(Unit* pmTarget, uint32 pmSpellId, Unit* pmCaster)
{
    if (!pmTarget)
    {
        return false;
    }
    uint32 result = 0;
    uint32 activeSpellId = HasAura(pmTarget, pmSpellId, pmCaster);
    if (activeSpellId > 0)
    {
        ObjectGuid ogCaster = ObjectGuid::Empty;
        if (pmCaster)
        {
            ogCaster = pmCaster->GetGUID();
        }
        if (Aura* targetAura = pmTarget->GetAura(activeSpellId, ogCaster))
        {
            result = targetAura->GetDuration();
        }
    }
    return result;
}

uint32 Script_Base::GetAuraStack(Unit* pmTarget, uint32 pmSpellId, Unit* pmCaster)
{
    if (!pmTarget)
    {
        return false;
    }
    uint32 result = 0;
    uint32 activeSpellId = HasAura(pmTarget, pmSpellId, pmCaster);
    if (activeSpellId > 0)
    {
        ObjectGuid ogCaster = ObjectGuid::Empty;
        if (pmCaster)
        {
            ogCaster = pmCaster->GetGUID();
        }
        if (Aura* targetAura = pmTarget->GetAura(activeSpellId, ogCaster))
        {
            result = targetAura->GetStackAmount();
        }
    }
    return result;
}

Player* Script_Base::GetNearbyHostilePlayer(Player* pmSearcher, float pmRange)
{
    std::list<Player*> players;
    Acore::AnyPlayerInObjectRangeCheck checker(pmSearcher, pmRange);
    Acore::PlayerListSearcher<Acore::AnyPlayerInObjectRangeCheck> searcher(pmSearcher, players, checker);
    Cell::VisitWorldObjects(pmSearcher, searcher, pmRange);
    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if (Player* eachPlayer = *itr)
        {
            if (eachPlayer->IsAlive())
            {
                if (pmSearcher->IsValidAttackTarget(eachPlayer))
                {
                    return eachPlayer;
                }
            }
        }
    }

    return nullptr;
}

Unit* Script_Base::GetNearbyHostileUnit(Player* pmSearcher, float pmRange)
{
    std::list<Creature*> creatureList;
    Acore::AnyUnitInObjectRangeCheck go_check(pmSearcher, pmRange);
    Acore::CreatureListSearcher<Acore::AnyUnitInObjectRangeCheck> go_search(pmSearcher, creatureList, go_check);
    Cell::VisitGridObjects(pmSearcher, go_search, pmRange);
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
                            if (pmSearcher->IsValidAttackTarget(hostileCreature))
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

Unit* Script_Base::GetAnyUnitInRange(Player* pmSearcher, float pmMinRange, float pmMaxRange)
{
    std::unordered_map<uint32, Unit*> unitsMap;

    std::list<Creature*> creatureList;
    Acore::AnyUnitInObjectRangeCheck go_check(pmSearcher, pmMaxRange);
    Acore::CreatureListSearcher<Acore::AnyUnitInObjectRangeCheck> go_search(pmSearcher, creatureList, go_check);
    Cell::VisitGridObjects(pmSearcher, go_search, pmMaxRange);
    if (!creatureList.empty())
    {
        for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
        {
            if (Unit* eachUnit = *itr)
            {
                float eachDistance = pmSearcher->GetDistance(eachUnit);
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
*/

