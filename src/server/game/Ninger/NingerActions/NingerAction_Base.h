#ifndef NINGER_ACTION_BASE_H
#define NINGER_ACTION_BASE_H

#ifndef DEFAULT_MOVEMENT_LIMIT_DELAY
# define DEFAULT_MOVEMENT_LIMIT_DELAY 5000
#endif

#include "Unit.h"
#include "Item.h"
#include "Player.h"
#include "NingerConfig.h"
#include "NingerManager.h"

enum NingerMovementType :uint32
{
    NingerMovementType_None = 0,
    NingerMovementType_Point,
    NingerMovementType_Chase,
};

class NingerMovement
{
public:
    NingerMovement(Player* pmMe);
    void ResetMovement();
    void Update(uint32 pmDiff);

    bool Chase(Unit* pmChaseTarget, float pmChaseDistanceMin, float pmChaseDistanceMax, uint32 pmLimitDelay = DEFAULT_MOVEMENT_LIMIT_DELAY);
    void MovePoint(Position pmTargetPosition, uint32 pmLimitDelay = DEFAULT_MOVEMENT_LIMIT_DELAY);
    void MovePoint(float pmX, float pmY, float pmZ, uint32 pmLimitDelay = DEFAULT_MOVEMENT_LIMIT_DELAY);
    void MoveTargetPosition();
    void MoveTargetPosition(float pmX, float pmY, float pmZ);

public:
    Player* me;
    ObjectGuid ogChaseTarget;
    Position positionTarget;
    uint32 activeMovementType;
    float chaseDistanceMin;
    float chaseDistanceMax;
};

class NingerAction_Base
{
public:
    NingerAction_Base(Player* pmMe);
    void Initialize();
    virtual void Reset();
    virtual void Update(uint32 pmDiff);
    virtual bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    virtual bool Tank(Unit* pmTarget, bool pmChase, bool pmAOE);
    virtual bool Heal(Unit* pmTarget);
    virtual bool Cure(Unit* pmTarget);
    virtual bool Buff(Unit* pmTarget);
    virtual bool Assist(Unit* pmTarget);
    virtual bool Revive(Player* pmTarget);
    virtual bool Petting(bool pmSummon = true);

    void PetAttack(Unit* pmTarget);
    void PetStop();

    Item* GetItemInInventory(uint32 pmEntry);
    bool UseItem(Item* pmItem, Unit* pmTarget);
    bool UseItem(Item* pmItem, Item* pmTarget);
    bool UseHealingPotion();
    bool UseManaPotion();
    bool SpellValid(uint32 pmSpellID);
    uint32 GetMaxRankSpellId(uint32 pmBaseSpellID, uint32 pmPlayerLevel);
    bool CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura = false, bool pmOnlyMyAura = false, bool pmClearShapeShift = false);
    void ClearShapeshift();
    void CancelAura(uint32 pmSpellID);    
    bool Eat(bool pmForce = false);
    bool Drink();
    bool Chase(Unit* pmTarget, float pmMinDistance, float pmMaxDistance);
    bool Follow(Unit* pmTarget, float pmDistance);
    void ChooseTarget(Unit* pmTarget);
    void ClearTarget();

    bool InitializeCharacter(uint32 pmTargetLevel);
    bool RandomTeleport();
    void TeleportTo(uint32 pmMapID, float pmDestX, float pmDestY, float pmDestZ);

    uint32 HasAura(uint32 pmSpellId, Unit* pmCaster = NULL);
    uint32 GetAuraDuration(uint32 pmSpellId, Unit* pmCaster = NULL);
    uint32 GetAuraStack(uint32 pmSpellId, Unit* pmCaster = NULL);

    Player* GetNearbyHostilePlayer(float pmRange = RANGE_HEAL_DISTANCE);
    Unit* GetNearbyHostileUnit(float pmRange = RANGE_HEAL_DISTANCE);
    Unit* GetAnyUnitInRange(float pmMinRange = 0.0f, float pmMaxRange = VISIBILITY_DISTANCE_NORMAL);

    Player* me;
    NingerMovement* rm;    

    uint32 maxTalentTab;
    float chaseDistanceMin;
    float chaseDistanceMax;
    int rti;    

private:
    void InitializeSpells();
    void InitializeTalents();
    void InitializeEquipments();
    bool EquipNewItem(uint32 pmItemEntry, uint8 pmEquipSlot);
    uint32 GetUsableArmorSubClass(Player* pmTargetPlayer);
    void TryEquip(Player* pmTargetPlayer, std::unordered_set<uint32> pmClassSet, std::unordered_set<uint32> pmSubClassSet, uint32 pmTargetSlot);
};
#endif
