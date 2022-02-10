#ifndef NINGER_ACTION_HUNTER_H
#define NINGER_ACTION_HUNTER_H

#include "NingerAction_Base.h"

enum HunterAspectType :uint32
{
    HunterAspectType_Hawk = 0,
    HunterAspectType_Monkey,
    HunterAspectType_Wild,
    HunterAspectType_Pack,
};

class NingerAction_Hunter :public NingerAction_Base
{
public:
    NingerAction_Hunter(Player* pmMe);
    void Prepare();

    uint32 aspectType;
};
#endif
