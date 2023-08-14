#ifndef NIER_ACTION_PRIEST_H
#define NIER_ACTION_PRIEST_H

#include "NierAction_Base.h"

class NierAction_Priest :public NierAction_Base
{
public:
	NierAction_Priest(Player* pmMe);
	void InitializeCharacter(uint32 pmTargetLevel, uint32 pmSpecialtyTabIndex);
	void ResetTalent();
	bool InitializeEquipments(bool pmReset);
	void Prepare();
    bool DPS(Unit* pmTarget, bool pmRushing, float pmDistanceMax, float pmDistanceMin, bool pmHolding, bool pmInstantOnly, bool pmChasing, bool pmForceBack);
	bool Heal(Unit* pmTarget, bool pmInstantOnly);
	bool ReadyTank(Unit* pmTarget);
	bool GroupHeal(Unit* pmTarget, bool pmInstantOnly);
	bool SimpleHeal(Unit* pmTarget, bool pmInstantOnly);
	bool Cure(Unit* pmTarget);
	bool Buff(Unit* pmTarget);
	bool Revive(Player* pmTarget);

	bool Heal_Discipline(Unit* pmTarget, bool pmInstantOnly);
	bool Heal_Holy(Unit* pmTarget, bool pmInstantOnly);

private:
    uint32 spell_shoot;

    uint32 spell_MindFlay;
    uint32 spell_CircleOfHealing;

    uint32 spell_Smite;
	uint32 spell_Renew;
	uint32 spell_LesserHeal;
    uint32 spell_ShadowWord_Pain;
    uint32 spell_MindBlast;
    uint32 spell_InnerFire;
    uint32 spell_ApolishDisease;
    uint32 spell_HolyFire;
    uint32 spell_ShadowProtection;
    uint32 spell_ShackleUndead;
    uint32 spell_MindSoothe;
    uint32 spell_PsychicScream;
    uint32 spell_Levitate;
    uint32 spell_DevouringPlague;
    uint32 spell_Fade;
    uint32 spell_Shadowfiend;
    uint32 spell_MindVision;
    uint32 spell_ShadowWord_Death;
    uint32 spell_MindControl;
    uint32 spell_PrayerOfMending;
    uint32 spell_FearWard;
    uint32 spell_PrayerOfHealing;
    uint32 spell_PrayerOfShadowProtection;
    uint32 spell_PrayerOfFortitude;
    uint32 spell_PrayerOfSpirit;
	uint32 spell_Heal;
	uint32 spell_GreaterHeal;
	uint32 spell_FlashHeal;
	uint32 spell_Resurrection;
	uint32 spell_CureDisease;
	uint32 spell_DispelMagic;
	uint32 spell_DivineSpirit;
	uint32 spell_PowerWord_Fortitude;
	uint32 spell_PowerWord_Shield;
	uint32 spell_Penance;
	uint32 spell_Weakened_Soul;
	uint32 spell_Prayer_of_Healing;
	uint32 spell_Prayer_of_Spirit;
	uint32 spell_Prayer_of_Fortitude;
	uint32 spell_InnerFocus;
	uint32 spell_PowerInfusion;
	uint32 spell_PainSuppression;
	uint32 spell_DesperatePrayer;
	uint32 spell_HolyNova;
	uint32 spell_Prayer_Of_Mending;
	uint32 spell_GuardianSpirit;
	uint32 aura_Surge_of_Light;
};
#endif
