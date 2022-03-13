#ifndef NINGER_ACTION_BASE_H
#define NINGER_ACTION_BASE_H

#ifndef DEFAULT_ACTION_LIMIT_DELAY
#define DEFAULT_ACTION_LIMIT_DELAY 5000
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
    NingerMovementType_Follow,
};

class NingerMovement
{
public:
    NingerMovement(Player* pmMe);
    void ResetMovement();
    void Update(uint32 pmDiff);

    bool Chase(Unit* pmChaseTarget, float pmChaseDistanceMin, float pmChaseDistanceMax, uint32 pmLimitDelay = DEFAULT_ACTION_LIMIT_DELAY);
    bool Follow(Unit* pmFollowTarget, float pmFollowDistanceMin, float pmFollowDistanceMax);
    void MovePoint(Position pmTargetPosition, uint32 pmLimitDelay = DEFAULT_ACTION_LIMIT_DELAY);
    void MovePoint(float pmX, float pmY, float pmZ, uint32 pmLimitDelay = DEFAULT_ACTION_LIMIT_DELAY);
    void MoveTargetPosition();
    void MoveTargetPosition(float pmX, float pmY, float pmZ);

public:
    Player* me;
    ObjectGuid ogChaseTarget;
    Position positionTarget;
    uint32 activeMovementType;
    float chaseDistanceMin;
    float chaseDistanceMax;
    bool chasePositionOK;
};

class NingerAction_Base
{
public:
    NingerAction_Base();
    virtual void Reset();
    virtual void Prepare();
    virtual void Update(uint32 pmDiff);
    virtual bool DPS(Unit* pmTarget, bool pmAOE, float pmChaseDistanceMin, float pmChaseDistanceMax);
    virtual bool Tank(Unit* pmTarget, bool pmAOE);
    virtual bool Heal(Unit* pmTarget);
    virtual bool SimpleHeal(Unit* pmTarget);
    virtual bool Cure(Unit* pmTarget);
    virtual bool Buff(Unit* pmTarget);
    virtual bool Assist(Unit* pmTarget);
    virtual bool Revive(Player* pmTarget);
    virtual bool Petting(bool pmSummon = true, bool pmReset = false);
    virtual void InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex);
    virtual void ResetTalent();
    virtual void InitializeEquipments(bool pmReset = false);

    void LearnTalent(uint32 pmTalentId, uint32 pmMaxRank = MAX_TALENT_RANK);
    void TrainSpells(uint32 pmTrainerEntry);
    void EquipRandomItem(uint32 pmEquipSlot, uint32 pmClass, uint32 pmSubclass, uint32 pmMinQuality, uint32 pmMaxRequiredLevel, uint32 pmModType);
    void PetAttack(Unit* pmTarget);
    void PetStop();
    bool UseItem(Item* pmItem, Unit* pmTarget);
    bool CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura = false, bool pmOnlyMyAura = false, bool pmClearShapeShift = false);
    void ClearShapeshift();
    void CancelAura(uint32 pmSpellID);
    bool Eat();
    bool Drink();
    bool Chase(Unit* pmTarget, float pmMinDistance, float pmMaxDistance);
    bool Follow(Unit* pmTarget, float pmMinDistance, float pmMaxDistance);
    bool RandomTeleport();

    void ChooseTarget(Unit* pmTarget);
    void ClearTarget();
    bool SpellValid(uint32 pmSpellID);
    Item* GetItemInInventory(uint32 pmEntry);
    Player* GetNearbyHostilePlayer(float pmRange = RANGE_MAX_DISTANCE);
    Unit* GetNearbyHostileUnit(float pmRange = RANGE_MAX_DISTANCE);
    Unit* GetAnyUnitInRange(float pmMinRange = 0.0f, float pmMaxRange = VISIBILITY_DISTANCE_NORMAL);

    Player* me;
    NingerMovement* rm;

    uint32 specialty;
    float chaseDistanceMin;
    float chaseDistanceMax;
    int rti;
};
#endif
