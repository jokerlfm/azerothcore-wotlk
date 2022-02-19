#include "NingerAction_Warlock.h"
#include "Group.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "Pet.h"

NingerAction_Warlock::NingerAction_Warlock() :NingerAction_Base()
{
    curseType = WarlockCurseType::WarlockCurseType_Weakness;
    curseDelay = 0;
    manaCheckDelay = 0;
    soulstoneDelay = 0;
    soulLinkDelay = 0;
    felArmorDelay = 0;
    wardDelay = 0;
    summonDelay = 0;
    soulHarvestDelay = 0;
    soulburnDelay = 0;
    soulFireDelay = 0;
    soulshatterDelay = 0;
    shadowfuryDelay = 0;
    conflagrateDelay = 0;
    immolateDelay = 0;
    shadowburnDelay = 0;
    chaosBoltDelay = 0;
    baneOfHavocDelay = 0;
    empoweredImpDelay = 0;
}
