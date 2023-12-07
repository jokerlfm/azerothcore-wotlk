#include "Nier_Hunter.h"

Nier_Hunter::Nier_Hunter() :Nier_Base()
{

}

void Nier_Hunter::Prepare()
{
    Nier_Base::Prepare();
}

void Nier_Hunter::Update(uint32 pDiff)
{
    Nier_Base::Update(pDiff);
}

void Nier_Hunter::Update_Online(uint32 pDiff)
{
    Nier_Base::Update_Online(pDiff);
}

void Nier_Hunter::InitializeCharacter()
{
    Nier_Base::InitializeCharacter();
}

bool Nier_Hunter::Tank(Unit* pTarget)
{
    return Nier_Base::Tank(pTarget);
}

bool Nier_Hunter::Heal(Unit* pTarget)
{
    return Nier_Base::Tank(pTarget);
}

bool Nier_Hunter::DPS(Unit* pTarget, Unit* pTank, bool pRushing)
{
    return Nier_Base::DPS(pTarget, pTank, pRushing);
}

bool Nier_Hunter::Buff()
{
    return Nier_Base::Buff();
}

bool Nier_Hunter::Cure()
{
    return Nier_Base::Cure();
}

bool Nier_Hunter::Revive()
{
    return Nier_Base::Revive();
}
