#include "NingerAction_Rogue.h"
#include "Group.h"
#include "Spell.h"
#include "GridNotifiers.h"

NingerAction_Rogue::NingerAction_Rogue() :NingerAction_Base()
{
    poisonStatus = 0;
    poisonDelay = 0;
    kickDelay = 0;
    sliceDelay = 0;
    adrenalineRushDelay = 0;
    dismantleDelay = 0;
    killingSpreeDelay = 0;
    evasionDelay = 0;
    sprintDelay = 0;
    revealingStrikeDelay = 0;
    recuperateDelay = 0;
}
