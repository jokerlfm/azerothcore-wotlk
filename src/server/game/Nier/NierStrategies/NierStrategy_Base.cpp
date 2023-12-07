#include "NierStrategy_Base.h"

#include "Player.h"
#include "Group.h"
#include "Pet.h"
#include "GridNotifiers.h"

NierStrategy_Base::NierStrategy_Base()
{
    rushing = false;
}

void NierStrategy_Base::Update(uint32 pDiff, Player* pMaster, std::unordered_set<Nier_Base*> pNierSet)
{
    Player* tank = nullptr;
    if (pMaster->groupRole == GroupRole::GroupRole_Tank)
    {
        tank = pMaster;
    }
    for (std::unordered_set<Nier_Base*>::iterator nit = pNierSet.begin(); nit != pNierSet.end(); nit++)
    {
        if (Nier_Base* eachNier = *nit)
        {
            if (eachNier->entityState == NierState::NierState_Online)
            {
                if (Player* eachNierPlayer = eachNier->me)
                {
                    if (!eachNierPlayer->isNier)
                    {
                        continue;
                    }
                    if (!eachNierPlayer->IsInSameGroupWith(pMaster))
                    {
                        // solo update 
                    }
                    else
                    {
                        if (eachNierPlayer->groupRole == GroupRole::GroupRole_Tank) {
                            tank = eachNierPlayer;
                        }
                    }
                }
            }
        }
    }
    if (pMaster->IsInCombat())
    {
        if (Group* group = pMaster->GetGroup())
        {
            Unit* skull = ObjectAccessor::GetCreature(*pMaster, group->GetGuidByTargetIcon(7));
            for (std::unordered_set<Nier_Base*>::iterator nit = pNierSet.begin(); nit != pNierSet.end(); nit++)
            {
                if (Nier_Base* eachNier = *nit)
                {
                    if (eachNier->entityState == NierState::NierState_Online)
                    {
                        if (Player* eachNierPlayer = eachNier->me)
                        {
                            if (!eachNierPlayer->isNier)
                            {
                                continue;
                            }
                            if (!eachNierPlayer->IsAlive() || eachNierPlayer->HasUnitState(UNIT_STATE_STUNNED))
                            {
                                continue;
                            }
                            if (eachNier->freezing)
                            {
                                continue;
                            }
                            if (eachNierPlayer->groupRole == GroupRole::GroupRole_Tank)
                            {

                            }
                            else if (eachNierPlayer->groupRole == GroupRole::GroupRole_Healer)
                            {
                                if (eachNier->Heal(tank))
                                {
                                    continue;
                                }
                                else if (eachNier->Heal(pMaster))
                                {
                                    continue;
                                }
                                else
                                {
                                    for (std::unordered_set<Nier_Base*>::iterator tit = pNierSet.begin(); tit != pNierSet.end(); tit++)
                                    {
                                        if (Nier_Base* eachTargetNier = *tit)
                                        {
                                            if (eachNier->Heal(eachTargetNier->me))
                                            {
                                                continue;
                                            }
                                        }
                                    }
                                }
                            }
                            else if (eachNierPlayer->groupRole == GroupRole::GroupRole_DPS)
                            {
                                if (eachNier->DPS(skull, tank, rushing))
                                {
                                    continue;
                                }
                            }
                            eachNier->Follow(pMaster);
                        }
                    }
                }
            }
        }
    }
    else
    {
        rushing = false;
        for (std::unordered_set<Nier_Base*>::iterator nit = pNierSet.begin(); nit != pNierSet.end(); nit++)
        {
            if (Nier_Base* eachNier = *nit)
            {
                if (eachNier->entityState == NierState::NierState_Online)
                {
                    if (eachNier->Cure())
                    {
                        continue;
                    }
                    if (eachNier->Buff())
                    {
                        continue;
                    }
                    if (eachNier->Revive())
                    {
                        continue;
                    }
                    if (eachNier->Rest())
                    {
                        continue;
                    }
                    if (eachNier->Follow(pMaster))
                    {
                        continue;
                    }
                }
            }
        }
    }
}
