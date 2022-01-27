#include "NingerAction_Hunter.h"
#include "Pet.h"
#include "Group.h"

NingerAction_Hunter::NingerAction_Hunter(Player* pmMe) :NingerAction_Base(pmMe)
{
    aspectType = HunterAspectType::HunterAspectType_Hawk;
}
