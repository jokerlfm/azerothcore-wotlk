#include "NingerAction_Hunter.h"
#include "Pet.h"
#include "Group.h"
#include "Spell.h"

NingerAction_Hunter::NingerAction_Hunter(Player* pmMe) :NingerAction_Base(pmMe)
{
    aspectType = HunterAspectType::HunterAspectType_Hawk;
}

void NingerAction_Hunter::Prepare()
{
    uint32 ammoEntry = 0;
    Item* weapon = me->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
    if (weapon)
    {
        if (const ItemTemplate* it = weapon->GetTemplate())
        {
            uint32 subClass = it->SubClass;
            uint8 playerLevel = me->getLevel();
            if (subClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_BOW || subClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_CROSSBOW)
            {
                if (playerLevel >= 40)
                {
                    ammoEntry = 11285;
                }
                else if (playerLevel >= 25)
                {
                    ammoEntry = 3030;
                }
                else
                {
                    ammoEntry = 2515;
                }
            }
            else if (subClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_GUN)
            {
                if (playerLevel >= 40)
                {
                    ammoEntry = 11284;
                }
                else if (playerLevel >= 25)
                {
                    ammoEntry = 3033;
                }
                else
                {
                    ammoEntry = 2519;
                }
            }
            if (ammoEntry > 0)
            {
                if (!me->HasItemCount(ammoEntry, 100))
                {
                    me->StoreNewItemInBestSlots(ammoEntry, 1000);
                }
            }
        }
    }
    Pet* checkPet = me->GetPet();
    if (checkPet)
    {
        checkPet->SetReactState(REACT_DEFENSIVE);
        std::unordered_map<uint32, PetSpell> petSpellMap = checkPet->m_spells;
        for (std::unordered_map<uint32, PetSpell>::iterator it = petSpellMap.begin(); it != petSpellMap.end(); it++)
        {
            if (it->second.active == ACT_DISABLED || it->second.active == ACT_ENABLED)
            {
                const SpellInfo* pS = sSpellMgr->GetSpellInfo(it->first);
                if (pS)
                {
                    std::string checkNameStr = std::string(pS->SpellName[0]);
                    if (checkNameStr == "Prowl")
                    {
                        checkPet->ToggleAutocast(pS, false);
                    }
                    else if (checkNameStr == "Phase Shift")
                    {
                        checkPet->ToggleAutocast(pS, false);
                    }
                    else if (checkNameStr == "Cower")
                    {
                        checkPet->ToggleAutocast(pS, false);
                    }
                    else if (checkNameStr == "Growl")
                    {
                        if (me->GetGroup())
                        {
                            checkPet->ToggleAutocast(pS, false);
                        }
                        else
                        {
                            checkPet->ToggleAutocast(pS, true);
                        }
                    }
                    else
                    {
                        checkPet->ToggleAutocast(pS, true);
                    }
                }
            }
        }
    }

    me->Say("Prepared", Language::LANG_UNIVERSAL);
}
